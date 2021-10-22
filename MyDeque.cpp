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
		
		while(temp != nullptr && temp -> getNext() != nullptr){
			temp = temp -> getNext();
		}

		pthread_mutex_lock(&main_lock);

		temp -> setNext(new DequeNode<T>(init));
		pthread_mutex_t* new_lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
		//pthread_mutex_init(new_lock, NULL);
		locks.push_back(new_lock);
		len++;
		pthread_mutex_unlock(&main_lock);

	}

	void PushAt(size_t indx, const T& item){
		//std::cout << indx << " " << len << "\n";

		DequeNode<T>* temp = start;

		while(temp != nullptr && indx > 0){
			temp = temp -> getNext();
			indx--;
		}	

		pthread_mutex_lock(locks[indx]);

		temp -> push_back(std::move(const_cast<T&>(item)));		
		
		pthread_mutex_unlock(locks[indx]);

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
	pthread_mutex_t main_lock;
};


Deque<int> deq(0);

void* thread_two_contest(void*){
	for(size_t i = 0; i < deq.getLen(); ++i){
		//std::cout << i << " ";
		deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);
	}
	//deq.printMe();
	return NULL;
}

void* thread_one_contest(void*){
	for(size_t i = 0; i < deq.getLen(); ++i){
		//std::cout << i << " ";
		deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);deq.PushAt(i, i);
	}
	//deq.printMe();
	return NULL;
}

int main(){
	for(size_t i = 1; i < 10; ++i){
		deq.PushNode(i);
	}
	pthread_t threads[3];

	//for(int i = 1; i < 10; ++i)
	//	deq.PushNode(i);
	
	pthread_create(&threads[0], NULL, thread_one_contest, NULL);
	pthread_create(&threads[1], NULL, thread_two_contest, NULL);
	pthread_create(&threads[2], NULL, thread_two_contest, NULL);


	//for(size_t i = 0; i < 10; ++i){
	//	deq.PushAt(i, i);
	//}

	//deq.PushAt(0, 99);
	
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_join(threads[2], NULL);
	
	sleep(1);

	deq.printMe();
	
	return 0;	
}