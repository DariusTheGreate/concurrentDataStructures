#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>

template<typename T>
class DequeNode{
public:
	DequeNode() : next(nullptr){
	}

	DequeNode(const T& init){
		items.emplace_back(init);
		next = nullptr;
	}

	void push_back(const T& item){
		items.emplace_back(std::move(const_cast<T&>(item)));
	}
	
	void setNext(DequeNode* ptr){
		next = ptr;
	}

	DequeNode* getNext(){
		return next;
	}

	void printMe(){
		for(int i = 0; i < items.size(); ++i){
			std::cout << "item - " << items[i] << " ";
		}

		//std::cout << "\n";
	}

private:
	DequeNode* next;
	std::vector<T> items;
};

template<typename T>
class Deque{
public:
	Deque(const T& init){
		start = new DequeNode<T>(init);
		
		pthread_mutex_t* new_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	
		locks.push_back(new_lock);
		
		len++;
		//start = new DequeNode<T>();
		//start -> setNext(new DequeNode<T>());
	}

	~Deque(){
		//clear nodes;
		//clear locks;
	}

	void PushNode(const T& init){
		DequeNode<T>* temp = start;
		size_t lock_number = 0;

		pthread_mutex_lock(locks[lock_number]);
		
		while(temp != nullptr && temp -> getNext() != nullptr){
			temp = temp -> getNext();
			
			pthread_mutex_unlock(locks[lock_number]);
			++lock_number;
			pthread_mutex_lock(locks[lock_number]);
		}
		
		pthread_mutex_unlock(locks[lock_number]);

		temp -> setNext(new DequeNode<T>(init));
		pthread_mutex_t* new_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
				
		locks.push_back(new_lock);
		len++;
	}

	void RemoveNodeAt(size_t indx){
		//std::cout << indx << " <- \n";
		if(indx <= 0)
			return;

		DequeNode<T>* temp = start;
		DequeNode<T>* temp_back = nullptr;
		DequeNode<T>* temp_front = nullptr;

		if(start -> getNext() != nullptr)
			temp_front = start -> getNext();

		size_t lock_number = 0;
		pthread_mutex_lock(locks[lock_number]);
		
		while(temp != nullptr && indx > 0 && temp -> getNext() != nullptr){
			//std::cout << "	***\n";
			temp_back = temp;

			temp = temp -> getNext();

			if(temp -> getNext() != nullptr)
				temp_front = temp -> getNext();

			indx--;

			pthread_mutex_unlock(locks[lock_number]);
			++lock_number;
			pthread_mutex_lock(locks[lock_number]);
		}
		
		if(lock_number - 1 > 0){
			//std::cout << "*locked prev*\n";
			pthread_mutex_lock(locks[lock_number - 1]);
		}

		if(lock_number + 1 < len){
			//std::cout << "*locked next*\n";
			pthread_mutex_lock(locks[lock_number + 1]);
		}
	
		temp_back -> setNext(temp_front);
	
		delete temp;

		pthread_mutex_unlock(locks[lock_number - 1]);
		pthread_mutex_unlock(locks[lock_number + 1]);
		pthread_mutex_unlock(locks[lock_number]);

		free(locks[indx]);
		locks.erase(locks.begin() + indx);
		
		len--;	
	}

	void PushAt(size_t indx, const T& item){
		DequeNode<T>* temp = start;
		
		size_t lock_number = 0;
		pthread_mutex_lock(locks[lock_number]);
		
		while(temp != nullptr && indx > 0 && temp -> getNext() != nullptr){
			temp = temp -> getNext();
			indx--;
			
			pthread_mutex_unlock(locks[lock_number]);
			++lock_number;
			pthread_mutex_lock(locks[lock_number]);
		}	
		
		temp -> push_back(std::move(const_cast<T&>(item)));		
		
		pthread_mutex_unlock(locks[lock_number]);
	}

	void TryPushAt(size_t indx);
	
	void printMe(){
		DequeNode<T>* temp = start;
		
		while(temp != nullptr && temp -> getNext() != nullptr){
			temp -> printMe();
			temp = temp -> getNext();
			std::cout << "\n";
		}

		temp -> printMe();
	}

	size_t getLen(){
		return len;
	}
	 
private:
	size_t len = 0;
	DequeNode<T>* start;
	std::vector<pthread_mutex_t*> locks;
};


Deque<int> deq(0);

void* thread_two_contest(void*){
	for(size_t i = 0; i < deq.getLen(); ++i){
		deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);
	}
	
	return NULL;
}

void* thread_one_contest(void*){
	for(size_t i = 0; i < deq.getLen(); ++i){
		deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);
	}

	
	return NULL;
}

void* thread_three_contest(void*){
	//deq.RemoveNodeAt(4);
	//deq.RemoveNodeAt(6);
	//deq.PushNode(10);
	return NULL;
}

int main(){
	for(size_t i = 1; i < 10; ++i){
		deq.PushNode(i);
	}
	
	//for(size_t i = 1; i < 10; ++i){
	//	deq.RemoveNodeAt(i);
	//}

	deq.RemoveNodeAt(6);
	
	pthread_t threads[4];

	
	pthread_create(&threads[0], NULL, thread_one_contest, NULL);
	pthread_create(&threads[1], NULL, thread_two_contest, NULL);
	pthread_create(&threads[2], NULL, thread_two_contest, NULL);
	pthread_create(&threads[3], NULL, thread_three_contest, NULL);


	//for(size_t i = 0; i < 10; ++i){
	//	deq.PushAt(i, i);
	//}

	//deq.PushAt(0, 99);
	
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
	pthread_join(threads[3], NULL);
	
	sleep(1);

	deq.printMe();
	
	return 0;	
}