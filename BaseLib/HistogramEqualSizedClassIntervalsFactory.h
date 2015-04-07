#ifndef HISTOGRAMEQUALSIZEDCLASSINTERVALSFACTORY_H
#define HISTOGRAMEQUALSIZEDCLASSINTERVALSFACTORY_H


#include "HistogramAbstractClassIntervalFactory.h"


namespace BaseLib { namespace histogram {


/**
 * @author Thomas Hennig
 * @brief implementation of \c histogram::ClassIntervalFactory that creates a set
 * of equal sized intervals
 */
template<typename T>
class EqualSizedClassIntervalsFactory: public AbstractClassIntervalFactory<T>{
					 
protected:
	virtual std::vector<T> determineLimits() const{
		
		std::vector<T> limits;
		
		T currentLimit = this->minValue;
		limits.push_back(currentLimit);
		
		T totalDistance = this->maxValue - this->minValue;					 
		size_t numberOfInnerLimits = this->numberOfClasses -1;

		for(size_t i = 0; i < numberOfInnerLimits; i++){
			
			size_t limitIndex = i+1;
			size_t maximumLimitIndex = this->numberOfClasses+1-1;
			
			// linear interpolation
			currentLimit = this->minValue + (totalDistance*limitIndex/maximumLimitIndex);
			limits.push_back(currentLimit);
		}
		
		currentLimit = this->maxValue;
		limits.push_back(currentLimit);
		
		return limits;
	}
};
	

	
} /*namespace histogram*/ } /*namespace BaseLib*/

#endif // HISTOGRAMEQUALSIZEDCLASSINTERVALSFACTORY_H
