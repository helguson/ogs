#ifndef HISTOGRAMGAMMADISTORTEDSIZECLASSINTERVALSFACTORY_H
#define HISTOGRAMGAMMADISTORTEDSIZECLASSINTERVALSFACTORY_H

#include "HistogramEqualSizedClassIntervalsFactory.h"


namespace BaseLib { namespace histogram {


/**
 * @author Thomas Hennig
 * @brief implementation of \c histogram::ClassIntervalFactory that creates a set
 * of intervals, whose limits are distored using a power function
 * 
 * assert \c T implements T pow(T,T)
 */
template<typename T>
class GammaDistortedSizeClassIntervalsFactory: public EqualSizedClassIntervalsFactory<T>{
	
public:
	// constructors and ddestructors
	GammaDistortedSizeClassIntervalsFactory(T const & gamma)
	:gamma(gamma)
	{	
	}
	
protected:
	// attributes
	T gamma;
	
	// methods
	virtual std::vector<T> determineLimits() const{
		
		std::vector<T> limitsWithEqualDistances(EqualSizedClassIntervalsFactory<T>::determineLimits());
		std::vector<T> limits;
		
		
		for(T limit: limitsWithEqualDistances){
			
			T mappedLimit = denormalize(
						applyGamma(
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
	T applyGamma(T const & value) const{
		
		return pow(value, this->gamma);
	}
};

	
} /*namespace histogram*/ } /*namespace BaseLib*/

#endif // HISTOGRAMGAMMADISTORTEDSIZECLASSINTERVALSFACTORY_H
