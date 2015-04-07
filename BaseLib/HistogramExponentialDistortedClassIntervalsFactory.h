#ifndef HISTOGRAMEXPONENTIALDISTORTEDCLASSINTERVALSFACTORY_H
#define HISTOGRAMEXPONENTIALDISTORTEDCLASSINTERVALSFACTORY_H


#include "HistogramEqualSizedClassIntervalsFactory.h"


namespace BaseLib { namespace histogram {


/**
 * @author Thomas Hennig
 * @brief implementation of \c histogram::ClassIntervalFactory that creates a set
 * of intervals, whose limits are distored using an exponential function
 * 
 * assert \c T implements T pow(T,T)
 */
template<typename T>
class ExponentialDistortedClassIntervalsFactory: public EqualSizedClassIntervalsFactory<T>{
	
public:
	// constructors and destructors
	ExponentialDistortedClassIntervalsFactory(T const & basis)
	:basis(basis)
	{	
	}
	
protected:
	// attributes
	T basis;
	
	// methods
	virtual std::vector<T> determineLimits() const{
		
		std::vector<T> limitsWithEqualDistances(EqualSizedClassIntervalsFactory<T>::determineLimits());
		std::vector<T> limits;
		
		
		for(T limit: limitsWithEqualDistances){
			
			T mappedLimit = denormalize(
						distord(
							normalize(limit)
						)
					);
			
			limits.push_back(mappedLimit);
		}
		
		return limits;
	}
	
private:
	// methods
	/**
	 * maps [minValue, maxValue] -> [0, 1]
	 */
	T normalize(T const & value) const{
		
		return (value-this->minValue)/(this->maxValue-this->minValue);
	}
	
	/**
	 * maps [0, 1] -> [minValue, maxValue]
	 */
	T denormalize(T const & value) const{
		
		return value*(this->maxValue-this->minValue) +this->minValue;
	}
	
	/**
	 * maps [0, 1] -> [0, 1]
	 */
	T distord(T const & value) const{
		
		// value in [0, 1]
		
		T r1 = pow(this->basis, value);
		// in [1, basis]
		
		
		T r2 = r1 - 1;
		// in [0, basis-1]
		
		T r3 = r2 / (this->basis-1);
		// in [0, 1]
		
		return r3;
	}
};

	
} /*namespace histogram*/ } /*namespace BaseLib*/


#endif // HISTOGRAMEXPONENTIALDISTORTEDCLASSINTERVALSFACTORY_H
