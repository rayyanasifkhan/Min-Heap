#include <cstdlib>
#include <iostream>
#include <vector>
#include <exception>
#include <stdexcept>
#include <unordered_map>
 
using namespace std;
 
template<typename TKey, typename TValue>
class minqueue
{
private:
	
	struct KeyValuePair {
		TKey Key;
		TValue Value;
	};
	
	KeyValuePair* HeapArray;
	int Capacity;
	int Size;
	unordered_map<TKey, int>  UMap;
 
	
	//
	// Helper functions:
	// 
  void _reset() {
		delete[] HeapArray;
		UMap.clear();
		Size = 0;
		Capacity = 0;
	}
	
	
	void _swap(int first, int second) {
		KeyValuePair temp = HeapArray[first];
		HeapArray[first] = HeapArray[second];
		HeapArray[second] = temp;
	}
	
	
	void _percolateDown(int &index) { // for removing: always sift down from top after removing and switching with the last element
		
		// first child index of index 0
		int childIndex = 2 * index + 1; 
		
		// minValue in each 'sub-tree' of three nodes
		int minIndex = index;
		
		// in array bounds
		while (childIndex < Size) { 
			
			// minValue, minKey so far in 'sub-tree'
			KeyValuePair min = HeapArray[index];
			
			// check children; if smaller value is found, update minValue, minIndex
			for (int i = 0; i < 2 && i + childIndex < Size; i++) { 
				if (HeapArray[childIndex + i].Value < min.Value ||
					 ((HeapArray[childIndex + i].Value == min.Value) && (HeapArray[childIndex + i].Key < min.Key))) {
					
					min.Value = HeapArray[childIndex + i].Value;
					min.Key = HeapArray[childIndex + i].Key;
					minIndex = childIndex + i;
				}
			}
			
			// no change in minValue, no need to update Heap; return
			if (min.Value == HeapArray[index].Value && min.Key == HeapArray[index].Key)
				return;
			
			_swap(index, minIndex);
			UMap[HeapArray[index].Key] = index;
			UMap[HeapArray[minIndex].Key] = minIndex;
			
			index = minIndex;
			childIndex = 2 * index + 1;
		}
	}
	
	void _percolateUp(int &index) { // for inserting: starting index to percolate up will always be the last index
		
		int parentIndex;
		
		// while top of the 'tree' is not reached
		while (index > 0) {
			parentIndex = (index - 1) / 2;
			
			// swap if either the value of index is less than parent's value OR 
			// if the values are same but the element at index's key is less than element at parentIndex's key
			// store new indices
			if (HeapArray[index].Value < HeapArray[parentIndex].Value ||
				 ((HeapArray[index].Value == HeapArray[parentIndex].Value) && (HeapArray[index].Key < HeapArray[parentIndex].Key))) {
				
				_swap(index, parentIndex);
				UMap[HeapArray[index].Key] = index;
				UMap[HeapArray[parentIndex].Key] = parentIndex;
			}
			
			// no swap made; return;
			else {
				return;
			}
			index = parentIndex;
		}
	}
	
	void _delete(TKey key) {
		int indexToDelete = UMap[key];
		if (indexToDelete == Size - 1) {
			Size--;
			return;
		}
		
		HeapArray[indexToDelete] = HeapArray[Size - 1];
		UMap[HeapArray[indexToDelete].Key] = indexToDelete;
		Size--;
		_percolateUp(indexToDelete);
		_percolateDown(indexToDelete);
		return;
	}
	

public:
  //
  // default constructor:
  //
  // Queue has a max capacity for efficient implementation.
  // This max capacity must be specified at queue creation.
  //
  minqueue(int capacity)
  {
		HeapArray = new KeyValuePair[capacity];
		Size = 0;
		Capacity = capacity;
  }
   
  //
  // fill constructor:
  //
  // This allows for the efficient O(N) construction of
  // a queue with an initial set of keys, all with the same
  // initial value.  The max capacity of the queue is 
  // set to the # of keys provided for initialization;
  // it is assumed the keys are in ascending order.
  //
  minqueue(vector<TKey> keys, TValue initialValue)
  {
		Capacity = keys.size();
		Size = keys.size();
		HeapArray = new KeyValuePair[Capacity];
		
		for (unsigned int i = 0; i < keys.size(); i++) { 
			HeapArray[i].Key = keys[i];
			UMap[keys[i]] = i;
			HeapArray[i].Value = initialValue;
		}
	}
   
   
  //
  // destructor:
  //
  virtual ~minqueue()
  {
		_reset();
  }
   
 
  //
  // empty:
  //
  // Returns true if empty, false if not.
  //
  bool empty()
  {
		return (Size == 0);
  }
 
 
  //
  // push:
  //
  // Inserts the given (key, value) pair into the queue such that 
  // pop always returns the pair with the minimum value.  If the 
  // key is *already* in the queue, it's value is updated to the
  // given value and the queue reordered.  If the key is not in
  // the queue, the (key, value) pairs is added and the queue
  // reordered.
  //
  // NOTE: if two keys have the same value, i.e. (key1, value) and
  // (key2, value), then those pairs are ordered into ascending value
  // by their key.
  //
  void pushinorder(TKey key, TValue value)
  {
		// element not present; insert it
		if (UMap.find(key) == UMap.end()) {
			
			// we need to insert a new (key, value) pair but the queue is full:
			if (Size == Capacity)
				throw runtime_error("minqueue::pushinorder: queue full");
			
			else {
				UMap[key] = Size;
				int indexToFix = Size;
				HeapArray[Size].Key = key;
				HeapArray[Size].Value = value;
				Size++;
				_percolateUp(indexToFix);
			}
		}
			
		// element already present; update
		else {
			_delete(key);
			UMap.erase(key);
			pushinorder(key, value);
		}
  }
 

 
  //
  // front:
  //
  // Returns the key at the front of the queue; does *not* pop the 
  // (key, value) pair.  Throws a logic_error exception if the queue
  // is empty.
  //
  TKey minfront()
  {
    if (empty())
    {
      throw logic_error("minqueue::minfront: queue empty");
    }
		return HeapArray[0].Key;
  }
 
 
  //
  // pop:
  //
  // Pops and discards the (key, value) pair at the front of the queue.
  // Throws a logic_error exception if the queue is empty.
  //
  void minpop()
  {
    if (empty())
    {
      throw logic_error("minqueue::minpop: queue empty");
    }
		
		UMap.erase(HeapArray[0].Key);
		_swap(0, Size - 1);
		Size--;
		int indexToFix = 0;
		_percolateDown(indexToFix);
  }
};