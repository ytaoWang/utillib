#ifndef __UTIL_HPP_
#define __UTIL_HPP_

#include <cassert>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <string>
#include <stdexcept>

/**
 * 1. operator[]: split read and write
 */
namespace libutil {

	bool isPrime(unsigned long number) {

		if(number %2 == 0)
			return false;
		
		for(unsigned long i = 3; i <= sqrt(number); i = i + 2) {
			if(number % i == 0 ) return false;
		}

		return true;
	}
	
	unsigned long getPrime(unsigned long max) {
		
		while(max > 1) {
			if(isPrime(max))
				return max;
			max --;
		}

		return 0;
	}

	unsigned long expandPrime(unsigned long min) {

		while(!isPrime(min++)) ;
		
		return min - 1;
	}

	size_t stringHash_(const char *p) {

		size_t code = 0;

		while( *p != '\0') {
			
			code = 31 * code + (*p);
			p++;
		}
	
		return code;
	}

	struct StringHash {
		size_t operator()(std::string str) const {
			return stringHash_(str.c_str());
		}
		size_t operator()(const size_t x) const {
			return x;
		}
	};

	
	template<typename Key, typename Value,class Hash=std::hash<Key>>
	class HashTable {
	private:
		typedef struct bucket_s{
			Key key;
			Value value;
			long coll; //collision bit
			unsigned int used:1; // used flag
			bucket_s():coll(0),used(0){}
		} bucket_t;

		bucket_t *buckets_;
		Hash hash_;
		float loadFactor_;
		size_t loadsize_; // the max idea number of key in the hashtable
		size_t  bucketsize_; // the bucket's size
		size_t count_; // the total number of key in the hashtable
		size_t occupancy_; // the total number of collision bits set in the hashtable
		const size_t initsize = 3;
		const size_t initprime = 101;
		const size_t maxsize = (size_t)-1; // max unsigned bit number 0xffffffff
		const size_t sign =  (1 << (8 * sizeof(size_t) - 1)); // only signed bit 0x80000000
		const size_t maxssize = maxsize - sign;  // max signed bit number 0x7ffffffff

		void initHash(const Key& key,size_t &seed,size_t &incr)  const
		{
			//hash(key) must be positive. Also, we must not use the sign bit, 
			//since that is used for the collision bit.
			seed = static_cast<size_t>(hash_(key)) & maxssize;
			incr = 1 + (seed * initprime) % (bucketsize_ - 1 );
		}
		
		void expand() 
		{
			size_t rawsize = expandPrime(bucketsize_+1);
			rehash(rawsize,false);
		}
	
		void rehash()
		{
			rehash(bucketsize_+1,false);
		}
	
		void rehash(size_t newsize,bool forceNewHashCode)
		{ 
			occupancy_ = 0;
			bucket_t *newBuckets = new bucket_t[newsize];

			for(size_t nb = 0; nb < bucketsize_; ++nb)  {
				bucket_t *oldb = buckets_ + nb;
				if(oldb->used != 0) {
					size_t hashcode = ((forceNewHashCode ? hash_(oldb->key): oldb->coll) & maxssize);
					putEntry(newBuckets,newsize,oldb->key,oldb->value,hashcode);
				}
			}

			delete [] buckets_;
			
			buckets_ = newBuckets;
			bucketsize_ = newsize;
			loadsize_ = loadFactor_ * newsize;
		}
	
	
		void putEntry(bucket_t* newBuckets, size_t newsize, const Key &key,const Value &value,size_t hashcode)
		{
			size_t seed = hashcode;
			size_t incr = 1 + (seed * initprime) % (newsize - 1 );
			size_t bucketNumber = seed % newsize;
		
			do {
			
				if(newBuckets[bucketNumber].used == 0) {
					newBuckets[bucketNumber].key = key;
					newBuckets[bucketNumber].value = value;
					newBuckets[bucketNumber].coll |= hashcode;
					newBuckets[bucketNumber].used = 1;
					return;
				}

				if(newBuckets[bucketNumber].coll >= 0 ) {
					newBuckets[bucketNumber].coll |= sign;
					occupancy_++;
				}

				bucketNumber = (bucketNumber + incr) % newsize;
			
			}while(true);
		
		}
		void insert(const Key &key, const Value &value, bool add)
		{
			if(count_ >= loadsize_) 
				expand();
			else if(occupancy_ > loadsize_ && count_ > 100)
				rehash();
			
			size_t seed,incr,entry;
			initHash(key,seed,incr);
			size_t bucketNumber = seed % bucketsize_;
			entry = 0;
			int emptySlotNumber = -1;

#ifdef DEBUG
			std::cout << "insert function:" ;
#endif

			do {
				//std::cout << "number:" << buckets_+bucketNumber << std::endl;
				// insert if there is a empty slot have been a collsion bit or hasn't been used

#ifdef DEBUG
				std::cout << std::endl << "bucketNumber:" << bucketNumber << ",key:" << buckets_[bucketNumber].key;
				std::cout << ",value:" << buckets_[bucketNumber].value << std::endl << ",insert key:" << key << ",seed:" << seed ;
#endif

				if(emptySlotNumber == -1 && buckets_[bucketNumber].used == 0 && buckets_[bucketNumber].coll < 0) {
					emptySlotNumber = bucketNumber;
				}

				if(buckets_ [bucketNumber].used == 0  && ( (buckets_[bucketNumber].coll & maxssize) == 0 )) {
					
					if(emptySlotNumber != -1)
						bucketNumber = emptySlotNumber;

					buckets_[bucketNumber].key  = key;
					buckets_[bucketNumber].value = value;
					buckets_[bucketNumber].coll |= seed;
					buckets_[bucketNumber].used = 1;
					count_++;
#ifdef DEBUG
					std::cout << "emptySlotNumber:" << emptySlotNumber << ",bucket: "<< bucketNumber << ",---end" << std::endl;
#endif
					return;

				} 
				
				if(buckets_[bucketNumber].used) {

					if(buckets_[bucketNumber].key == key ) {
						if(add) {
							buckets_[bucketNumber].value = value;
						}
#ifdef DEBUG
						std::cout << "bucketNumber:" << bucketNumber << "---end" << std::endl;
#endif
						return;
					}  else {
						if(buckets_[bucketNumber].coll >= 0 )  {
							buckets_[bucketNumber].coll  |= sign;
							occupancy_ ++;
						}
					}

				}

				bucketNumber = (bucketNumber + incr) % bucketsize_;
				entry ++;

			}while(entry < bucketsize_);

			if(emptySlotNumber != -1) {
				buckets_[bucketNumber].key = key;
				buckets_[bucketNumber].value = value;
				buckets_[bucketNumber].coll |= seed;
				buckets_[bucketNumber].used = 1;
			}
			
			assert(0);
		}

		bool containsKey(const Key &key,Value &value)
		{
			size_t seed,incr,entry;
			initHash(key,seed,incr);
			size_t bucketNumber = seed % bucketsize_;
			entry = 0;
			bucket_t *b;

			do {

				b = &buckets_[bucketNumber];

				if(buckets_[bucketNumber].used == 0) {
					return false;
					
				} else {
					
					if((buckets_[bucketNumber].used != 0) && (buckets_[bucketNumber].key == key ))   {	
						value = buckets_[bucketNumber].value;
						return true;
					}
				}

				bucketNumber = (bucketNumber + incr) % bucketsize_;
				entry ++;
			}while(b->coll < 0 && entry < bucketsize_);
			
			return false;
		}
		
	public:
		HashTable(const Hash& h): HashTable(0,1.0f) {
			hash_ = h;
		}

		HashTable():HashTable(0,1.0f){}
		HashTable(int capacity, float loadFactor):count_(0),occupancy_(0) 
		{
			assert(capacity >= 0);
			assert(loadFactor >=0 && loadFactor <= 1.0f );
			
			loadFactor_ = 0.72f * loadFactor;
			double rawsize = capacity / loadFactor_;
			assert(rawsize <= maxsize);
			bucketsize_ = rawsize > initsize ? getPrime(rawsize) : initsize;
			loadsize_ = bucketsize_ * loadFactor_;
			buckets_ = new bucket_t[bucketsize_];
			//memset(buckets_,0,sizeof(bucket_t)*bucketsize_);
		}
		
		size_t count() const { return count_; }
				
		bool containsKey(const Key & key) 
		{
			size_t seed,incr,entry;
			initHash(key,seed,incr);
			size_t bucketNumber = seed % bucketsize_;
			entry = 0;
			bucket_t *b;

			do {
				b = &buckets_[bucketNumber];

				if(buckets_[bucketNumber].used == 0) {
					return false;

				} else {
					if(buckets_[bucketNumber].key == key )
						return true;
				}

				bucketNumber = (bucketNumber + incr) % bucketsize_;
				entry ++;
			}while(b->coll < 0 && entry < bucketsize_);		
			return false;
		}

		bool contains(const Key& key,const Value &value)   
		{
			Value v;
			if(!containsKey(key,v)) return false;
			return value == v;
		}
	private:
		class ProxyHT {
			friend class HashTable<Key, Value, Hash>;
			friend std::ostream &operator<<(std::ostream &os,const ProxyHT &p)
			{
				return p.write(os);
			}
		private:

			HashTable<Key,Value,Hash>   *pHt_;
			Key value_;
			ProxyHT() {}
			ProxyHT(HashTable<Key,Value,Hash> *pHt, const Key &key):pHt_(pHt) {
				value_ = key;
			}

			std::ostream& write(std::ostream& os) const {
				os << pHt_->read(value_);
				return os;
			}
			
		public:
			//reading
			operator Value() const {
				return pHt_->read(value_);
			}
			
			const Value& operator=(const Value& v) {
				return pHt_->write(value_ , v);
			}

			bool operator==(const Value &v) {

				/*
				return pHt_->read(value_) == v;
				try {
					return pHt_->read(value_) == v;
				} catch(const std::exception&) {
					return false;
				}
				
				return false;
				*/
				return pHt_->contains(value_,v);
			}

		public:
			~ProxyHT() {}
		};

	public:
		ProxyHT operator[](const Key &key) {
			return ProxyHT(this,key);
		}

	private:
		/*
		friend std::ostream &operator<<(std::ostream &os,const HashTable::ProxyHT&p) 
		{
			return p.write(os);
		}
		*/

		const Value& write(const Key &key,const Value &value) {
			add(key,value);
			return value;
		}

		const Value& read(const Key &key) const
		{
			size_t seed,incr,entry;
			entry = 0;
			bucket_t *b;
			initHash(key,seed,incr);
			size_t bucketNumber = seed % bucketsize_;
			
			do {
				b = &buckets_[bucketNumber];
				
#ifdef DEBUG
				std::cout << std::endl <<"bucketNumber:" << bucketNumber << ",key:" << key << ",seed:" << seed; // << std::endl;
				std::cout << ",key:" << buckets_[bucketNumber].key << ",value:" <<  buckets_[bucketNumber].value;
				std::cout <<",coll:" << buckets_[bucketNumber].coll << std::endl;
#endif
				if(buckets_[bucketNumber].used == 0) {
#ifdef DEBUG
					std::cerr << "bucketNumber:" << bucketNumber << " is not used." << std::endl;
#endif
					throw std::out_of_range("no entry for this key.");
				}
				
				if((buckets_[bucketNumber].coll & maxssize) == seed && buckets_[bucketNumber].key == key ) {
					return buckets_[bucketNumber].value;
				}

				bucketNumber = (bucketNumber + incr) % bucketsize_;
				entry ++;

			} while(entry < bucketsize_  && b->coll < 0); //buckets_[bucketNumber].coll < 0 && 
			
			throw std::out_of_range ("no entry for this key");
			return buckets_[0].value;
		}

	public:
		void add(const Key& key,const Value& value)
		{
			insert(key,value,true);
		}

		void  remove(const Key& key) 
		{
			size_t seed,incr,entry;
			initHash(key,seed,incr);
			size_t bucketNumber = seed % bucketsize_;
			entry = 0;
			bucket_t *b;

			do {

				b = &buckets_[bucketNumber];
				if(buckets_[bucketNumber].used == 0) {
					return;

				} else {
				
					if((buckets_[bucketNumber].coll & maxssize) == seed && buckets_[bucketNumber].key == key ) {

						//memset(&buckets_[bucketNumber].key,0,sizeof(Key));
						//memset(&buckets_[bucketNumber].value,0,sizeof(Value));
						buckets_[bucketNumber].used = 0;
						buckets_[bucketNumber].coll &= sign;
						count_ --;
						return;
					}
				}

				bucketNumber = (bucketNumber + incr) % bucketsize_;
				entry ++;
			}while(b->coll < 0 && entry < bucketsize_);

		}

		~HashTable() 
		{
			delete []buckets_;
		}
		
	};
}

#endif
