#pragma once

#include "CoreMinimal.h"
#include "HAL/ThreadSafeBool.h"


class FVoxelUtilities
{
public:
	static FORCEINLINE FIntVector VoxelPosToChunkIndex(const FIntVector& VoxelPos)
	{
		//-1 / 16 needs to be -1, not zero
		auto CustomDiv = [](int Val, int Div)
		{
			const int Result = Val / Div;
			return Div * Result == Val ? Result : Result - ((Val < 0) ^ (Div < 0));
		};

		return FIntVector(CustomDiv(VoxelPos.X, VOX_CHUNKSIZE)
			, CustomDiv(VoxelPos.Y, VOX_CHUNKSIZE)
			, CustomDiv(VoxelPos.Z, VOX_CHUNKSIZE));
	}

	static FORCEINLINE FIntVector VoxelPosToLocalPos(const FIntVector& VoxelPos)
	{
		auto CustomModulo = [](int Val, int Div) {const int Result = Val % Div; return Result < 0 ? Result + Div : Result; };
		return FIntVector(CustomModulo(VoxelPos.X, VOX_CHUNKSIZE)
			, CustomModulo(VoxelPos.Y, VOX_CHUNKSIZE)
			, CustomModulo(VoxelPos.Z, VOX_CHUNKSIZE));
	}
};

class FSpinLock
{
	FThreadSafeBool SafeBool;
public:
	void Lock()
	{
		while (SafeBool.AtomicSet(true));
	}

	void Unlock()
	{
		SafeBool = false;
	}
};

class FScopeSpinLock
{
	FSpinLock& Lock;

public:
	FScopeSpinLock(FSpinLock& Lock)
		: Lock(Lock)
	{
		Lock.Lock();
	}

	~FScopeSpinLock()
	{
		Lock.Unlock();
	}
};

//Just very basic linked list
template<typename T>
class TSimpleLinkedList
{
	struct InternalNode
	{
		InternalNode* Next = nullptr;
		T Data;
	};

	InternalNode* StartNode;
	InternalNode* LastNode;

	FSpinLock Lock;

public:
	TSimpleLinkedList()
	{
		StartNode = nullptr;
		LastNode = nullptr;
	}

private:
	//Iterates through all nodes
	TArray<InternalNode*> ToArray()
	{
		TArray<InternalNode*> Result;
		if (StartNode == nullptr || LastNode == nullptr)
		{
			check(LastNode == nullptr);
			check(StartNode == nullptr);

			UE_LOG(LogTestVoxel, Verbose, TEXT("ToArray() called without any nodes."));

			return Result;
		}

		InternalNode* NextNode = StartNode;
		while (NextNode)
		{
			Result.Add(NextNode);
			NextNode = NextNode->Next;
		}

		return Result;
	}

	//Adds a node and return the added node. Thread safe
	InternalNode* AddInternal()
	{
		FScopeSpinLock sl(Lock);

		//This is first node
		if (StartNode == nullptr)
		{
			StartNode = new InternalNode();
			LastNode = StartNode;

			return StartNode;
		}
		else
		{
			auto NewNode = new InternalNode();
			LastNode->Next = NewNode;
			LastNode = NewNode;

			return NewNode;
		}
	}

public:
	//Adds a node
	void Add(T Data)
	{
		InternalNode* Node = AddInternal();
		Node->Data = Data;
	}

	//Deletes every node and pops the data. Thread safe
	//SharedPtr for pointer safety
	TArray<T> Pop()
	{
		FScopeSpinLock sl(Lock);

		TArray<T> Result;

		auto AllNodes = ToArray();

		for (InternalNode* Node : AllNodes)
		{
			Result.Add(Node->Data);
			delete Node;
		}

		StartNode = nullptr;
		LastNode = nullptr;

		return Result;
	}

	//Not thread safe, but this function is expected to call every ticks
	bool IsEmpty()
	{
		return !(StartNode || LastNode);
	}

	~TSimpleLinkedList()
	{
		Pop();
	}
};
