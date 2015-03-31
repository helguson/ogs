#ifndef INTERVAL_H
#define INTERVAL_H


namespace BaseLib{

/**
 * @author Thomas Hennig
 * @brief one dimensional interval
 * 
 * assert T implements copy constructor T(T const &)
 * assert T implements bool operator<(T const &, T const &)
 * assert T implements bool operator<=(T const &, T const &)
 * assert T implements std::ostream & operator<<(std::ostream &, T const &)
 */
template<typename T>
class Interval{
	
public:
	// constructors & destructors
	Interval(T const & lowerLimit, bool lowerLimitIsInclusive, T const & upperLimit, bool upperLimitIsInclusive)
	:
		lowerLimit(lowerLimit),
		lowerLimitIsInclusive(lowerLimitIsInclusive),
		upperLimit(upperLimit),
		upperLimitIsInclusive(upperLimitIsInclusive)
	{}
	
	// getters and setters
	T const getLowerLimit() const{
		return this->lowerLimit;
	}
	
	T const getUpperLimit() const{
		return this->upperLimit;
	}
	
	bool isLowerLimitInclusive() const{
		return this->lowerLimitInclusive;
	}
	
	bool isUpperLimitInclusive() const{
		return this->upperLimitIsInclusive;
	}
	
	// methods
	bool contains(T const & value) const{
		
		bool satisfiesLowerLimit = this->lowerLimitPredicate(value);
		bool satisfiesUpperLimit = this->upperLimitPredicate(value);
		
		return  satisfiesLowerLimit && satisfiesUpperLimit;
	}
	
	std::string toString() const{
		
		std::stringstream stream;
		
		stream << ((this->lowerLimitIsInclusive)? "[":"(");
		stream << this->lowerLimit;
		stream << ", ";
		stream << this->upperLimit;
		stream << ((this->upperLimitIsInclusive)? "]":")");
		
		return stream.str();
	}
	
	// aka all elements of interval are smaller than rhs
	bool operator< (T const & rhs) const{
		
		bool satisfiesLowerLimit = this->lowerLimitPredicate(rhs);
		bool satisfiesUpperLimit = this->upperLimitPredicate(rhs);
		
		return  satisfiesLowerLimit && !satisfiesUpperLimit;
	}
	
	// aka all elements of interval are greater than rhs
	bool operator> (T const & rhs) const{
		
		bool satisfiesLowerLimit = this->lowerLimitPredicate(rhs);
		bool satisfiesUpperLimit = this->upperLimitPredicate(rhs);
		
		return  !satisfiesLowerLimit && satisfiesUpperLimit;
	}
	
	T getWidth() const{
		
		return this->upperLimit - this->lowerLimit;
	}
	
private:
	// attributes
	T lowerLimit;
	bool lowerLimitIsInclusive;
	
	T upperLimit;
	bool upperLimitIsInclusive;
	
	// methods
	bool lowerLimitPredicate(T const & value) const{
		
		bool result;
		
		if(this->lowerLimitIsInclusive){
			result = this->lowerLimit <= value;
		}
		else{
			result = this->lowerLimit < value;
		}
		
		return result;
	}
	
	bool upperLimitPredicate(T const & value) const{
		
		bool result;
		
		if(this->upperLimitIsInclusive){
			result = value <= this->upperLimit;
		}
		else{
			result = value < this->upperLimit;
		}
		
		return result;
	}
};

template<typename T>
std::ostream & operator<<(std::ostream & stream, Interval<T> const & interval){
	
	return stream << interval.toString();
}

// aka lhs is smaller than every element of rhs
template<typename T>
bool operator< (T const & lhs, Interval<T> const & rhs){
	
	return rhs > lhs;
}

// aka lhs is greater than every element of rhs
template<typename T>
bool operator> (T const & lhs, Interval<T> const & rhs){
	
	return rhs < lhs;
}



} /*namespace BaseLib*/

#endif // INTERVAL_H
