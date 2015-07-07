#ifndef __TREE_HPP_
#define __TREE_HPP_
#include <stdexcept>
#include <iterator>
#include <cassert>
#include <iostream>

namespace tree {
	
	template<typename T>
	class Tree;

	template<typename T>
	class NodeIterator;

	template<typename T>
	class ChildIterator;

	template<typename T>
	class SiblingIterator;

	template<typename T>
	class TreeNode {
		friend class Tree<T>;
		friend class NodeIterator<T>;
		friend class ChildIterator<T>;
		friend class SiblingIterator<T>;
	public:
		typedef TreeNode<T> value_type;
		typedef TreeNode<T>* pointer;
		typedef TreeNode<T>& reference;
		typedef NodeIterator<T> node_iterator;
		typedef ChildIterator<T> child_iterator;
		typedef SiblingIterator<T> sibling_iterator;
		typedef NodeIterator<T> iterator;
	private:
		pointer parent;
		pointer child_prev;
		pointer child_next;
		pointer sibling_prev;
		pointer sibling_next;
		pointer prev;
		pointer next;

		void addList(pointer node) {
			 
			pointer prev;
				
			assert(node != NULL);

			prev = this->prev;

			if(prev) {
				prev->next = node;
				this->prev = node;
				node->prev = prev;
				node->next = this;
			}				
		}
			
	public:

		TreeNode():parent(NULL),child_prev(this),child_next(this),sibling_prev(this),sibling_next(this),prev(this),next(this) {

		}
		
		TreeNode(const T& t):parent(NULL),child_prev(this),child_next(this),sibling_prev(this),sibling_next(this),prev(this),next(this),value_(t)
		{
				
		}

		iterator node_begin() { return next; }
		iterator node_end() { return this;}
			
		child_iterator child_begin() {return this->child_next;}
		child_iterator child_end() { return this;}

		sibling_iterator sibling_begin()   { 			
			if(parent)
				return parent->child_next; 
			return NULL;
		}

		sibling_iterator sibling_end() { return parent; }
			
		void setParent(pointer p) {
			parent = p;
		}

		pointer getParent(void) const {
			return parent;
		}
	
		bool operator==(const reference n2)  const{
		
			if(this == &n2) return 1;
		
			return value_ == n2.value_;
		}

		bool operator!=(const reference n2) const {
		
			if(this != &n2) return 1;
		
			return value_ != n2.value_;
		}
			
		std::ostream& writeNode(std::ostream &os,pointer sentiel)  {

			pointer iter = this;
				
			do {
				if(iter != sentiel) {
					os << "node:" << iter->value_ ;
					
					if(iter->parent)
						os  << ",parent:" << iter->parent->value_ << std::endl;
					else
						os << ",it's root" << std::endl;
				}

				iter = iter->next;

			} while(iter != this);

			return os;
		}

		std::ostream& writeParent(std::ostream &os) {
				
			if(this->parent)
				os << "parent:" << parent->value_ << std::endl;
			else
				os << "no parent,it's root" << std::endl;
			return os;
		}			
			
		std::ostream& writeChild(std::ostream &os)  {
			pointer iter = this->child_next;
				
			do {
				os << "child:" << iter->value ;
					
				if(iter->parent)
					os  << ",parent:" << iter->parent->value << std::endl;
				else
					os << ",it's root" << std::endl;

				iter = iter->sibling_next;
			} while(iter != this);

			return os;
		}
			
		bool hasChild(void) const {
				
			return this->child_next != this;
		}
			
		void removeChild(pointer child)  {

			pointer prev;
			pointer next;

			assert(child != NULL);
			child->setParent(NULL);
				
			prev = child->sibling_prev;
			next = child->sibling_next;
				
			if(child->hasChild()) throw std::logic_error("node has child,can't remove it ");
				
			if(prev) {
				if(prev != this) 
					prev->sibling_next = next;
				else //it's parent
					prev->child_next = next;	
			}
	
			if(next) {
				if(next != this)
					next->sibling_prev = prev;
				else
					next->child_prev = prev;
			}

			child->sibling_prev = child;
			child->sibling_next = child;
				
			//delete this node from 
			prev = child->prev;
			next = child->next;

			prev->next = next;
			next->prev = prev;
			child->next = child;
			child->prev = child;
			
			//delete child;
		}

		void addChild(pointer child) {

			pointer next;
			pointer prev;

			assert(child != NULL);
			child->setParent(this);
	
			prev = this->child_prev;
			next = this->child_next;

			if(prev) {
				child->sibling_prev = prev;
					
				if(prev != this)
					prev->sibling_next = child;
				else  // it's parent
					prev->child_next = child;
			}
				
			child->sibling_next = this;
			this->child_prev = child;

			addList(child);
		}

		bool getChild(const T &t,pointer &node)
		{
			pointer iter = this->child_next;

			do {
				if(iter->value_ == t) {
					node = iter;
					return true;
				}
				iter = iter->sibling_next;
			} while(iter != this);
				
			return false;
		}
			
		bool getNode(const T &t,pointer &node,const pointer sentiel)
		{
			pointer iter = this;
			do {
				if(iter != sentiel && iter->value_ == t) {
					node = iter;
					return true;
				}
				iter = iter->next;
			} while(iter != this);

			return false;
		}

	public:
		T value_;			
	};
	
	//////////////////// iterator //////////////////////////////////////////////
	template<typename T>
	class NodeIterator : public std::iterator<std::forward_iterator_tag,T> {
	private:
		typedef NodeIterator<T> self;
		TreeNode<T> *node_;
		void incr_self() {
			node_ = node_->next;
		}

		void decr_self() {
			node_ = node_->prev;
		}

	public:
		NodeIterator():node_(0) {}
		NodeIterator(TreeNode<T> *x):node_(x) { }
		~NodeIterator() {}

		T& operator*() const {return node_->value_;}
		T* operator->() const {return &(operator*());}
			
		bool operator==(const self& x) const {
			return *node_ == *(x.node_);
		}

		bool operator!=(const self& x) const {
			return *node_ != *(x.node_);
		}
			
		self& operator++() {incr_self();return *this;}
		self& operator++(int) {self __tmp = *this; incr_self(); return __tmp;}

		self& operator--() {decr_self(); return *this;}
		self& operator--(int) {self __tmp = *this;decr_self();return __tmp;}
	};

	template<typename T>
	class ChildIterator: public std::iterator<std::forward_iterator_tag,T> 
	{
	private:
		typedef ChildIterator<T> self;
		TreeNode<T> *node_;
		void incr_self() {
			node_ = node_->sibling_next;
		}

		void decr_self() {
			node_ = node_->sibling_prev;
		}

	public:
		ChildIterator():node_(0) {}
		ChildIterator(TreeNode<T> *x):node_(x) {}
		~ChildIterator() {}

		T& operator*() const {return node_->value_;}
		T* operator->() const {return &(operator*());}
			
		bool operator==(const self& x) const {
			return node_ == x.node_;
		}

		bool operator!=(const self& x) const {
			return node_ != x.node_;
		}
			
		self& operator++() {incr_self();return *this;}
		self& operator++(int) {self __tmp = *this; incr_self(); return __tmp;}

		self& operator--() {decr_self(); return *this;}
		self& operator--(int) {self __tmp = *this;decr_self();return __tmp;}

	};

	template<typename T>
	class SiblingIterator: public std::iterator<std::forward_iterator_tag,T> {
	private:
		typedef SiblingIterator<T> self;
		TreeNode<T> *node_;
		void incr_self() {
			node_ = node_->sibling_next;
		}

		void decr_self() {
			node_ = node_->sibling_prev;
		}

	public:
		SiblingIterator():node_(0) {}
		SiblingIterator(TreeNode<T> *x):node_(x) {
		}

		~SiblingIterator() {}

		T& operator*() const {
			return node_->value_;
		}

		T* operator->() const {return &(operator*());}
			
		bool operator==(const self& x) const {
			return node_ == x.node_;
		}

		bool operator!=(const self& x) const {
			return node_ != x.node_;
		}
			
		self& operator++() {incr_self();return *this;}
		self& operator++(int) {self __tmp = *this; incr_self(); return __tmp;}

		self& operator--() {decr_self(); return *this;}
		self& operator--(int) {self __tmp = *this;decr_self();return __tmp;}

	};
	
	template<typename T>
	class Tree {
	private:
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef typename TreeNode<T>::pointer node_pointer;
		typedef typename TreeNode<T>::value_type node_type;
	public:
		typedef typename TreeNode<T>::iterator iterator;
		typedef typename TreeNode<T>::node_iterator node_iterator;
		typedef typename TreeNode<T>::child_iterator child_iterator;
		typedef typename TreeNode<T>::sibling_iterator sibling_iterator;
	private:
		size_t count;
		node_pointer root_;
		node_pointer sentiel_; //
		template<typename V>
		friend std::ostream &operator<<(std::ostream &os,const Tree<V> &tree);

	public:
		size_t size() const { return count; }
		
		Tree():count(0),root_(NULL) {
			sentiel_ = new node_type();
		}

		void getParent(const_reference value,T &v)
		{
			node_pointer p;
			if(root_->getNode(value,p) && p->getParent()) {
				v =  p->getParent()->value_;
			}
		}
		
		bool getChild(const_reference value,node_pointer &p)
		{
			return root_ ?(root_->getChild(value,p)?:sentiel_->getNode(value,p,sentiel_)):false;
			//return root_?root_->getChild(value,p):false;
		}
		
		iterator begin() {return sentiel_->node_begin();}
		iterator end() {return sentiel_->node_end();}

		child_iterator child_begin(const_reference value) {
			node_pointer p;
			if(sentiel_->getNode(value,p)) 
				return p->child_begin();
			
			else
				return sentiel_->child_begin();

		}

		child_iterator child_end(const_reference value) {
						node_pointer p;
			if(sentiel_->getNode(value,p)) 
				return p->child_end();
			else
				return sentiel_->child_begin();
		}
		
		sibling_iterator sibling_begin(const_reference value) {
			node_pointer p;
			if(sentiel_->getNode(value,p)) 
				return p->sibling_begin();
			
			else
				return sentiel_->sibling_begin();
		}

		sibling_iterator sibling_end(const_reference value) {
			node_pointer p;
			if(sentiel_->getNode(value,p)) 
				return p->sibling_end();
			else
				return sentiel_->sibling_begin();
		}
		

		void addChild(const_reference value)
		{
			node_pointer node;
			node  = new node_type(value);
			if(0 == count) {
				assert(root_ == NULL);
				root_ = node;
				sentiel_->addChild(node);
				node->setParent(NULL);
			} else {
				root_->addChild(node);
			}
			count++;
		}

		void addChild(const_reference parent,const_reference child)
		{
			node_pointer p;
			if(!getChild(parent,p))  throw std::out_of_range("no entry for this parent");
			addChild(p,child);
		}

		void addChild(node_pointer parent,const_reference value)
		{
			node_pointer node;
			node  = new node_type(value);
			assert(parent != NULL);
			parent->addChild(node);
			count++;
		}

		void removeChild(node_pointer parent,const_reference value)
		{
			node_pointer p;

			if(!parent->getNode(value,p,sentiel_))
				throw std::out_of_range("no entry for this key");

			if(p->hasChild())
				throw std::logic_error("it's can't remove,please remove child first");
			
			if(!parent && value == root_->value_) {
				delete root_;
				root_ = NULL;
			} else {
				if(parent && !parent->getChild(value,p)) 
					throw std::out_of_range("no entry for this key.");
				parent->removeChild(p);
				delete p;
				p = NULL;
			}

			count--;
		}

		void removeChild(const_reference parent,const_reference child)
		{
			node_pointer p;
			
			if(!getChild(parent,p)) throw std::out_of_range("no entry for this key.");
			
			removeChild(p,child);
		}

		void removeChild(const_reference value)
		{
			removeChild(sentiel_,value);
		}

		~Tree() 
		{
			node_pointer n,iter;
			for(n = sentiel_->next, iter = n->next; iter != sentiel_; n = iter, iter = n->next)
				delete n;
			delete n;
			delete sentiel_;
		}
		
	};

	template<typename T>
	std::ostream &operator<<(std::ostream &os,const Tree<T> &tree) 
	{
		return tree.root_->writeNode(os,tree.sentiel_);
	}
}


#endif
