#ifndef HISTOGRAMEQUALSIZEDCLASSINTERVALSFACTORY_H
#define HISTOGRAMEQUALSIZEDCLASSINTERVALSFACTORY_H

#include <utility>

#include "HistogramClassIntervalFactory.h"

namespace BaseLib { namespace histogram {



/**
 * @author Thomas Hennig
 * 
 */
template<typename T>
class EqualSizedClassIntervalsFactory: public ClassIntervalFactory<T>{
	
public: // methods
	virtual std::vector<Interval<T>> createClassIntervals() const override{
					 
		std::vector<Interval<T>> intervals;
					 
		std::vector<T> limits = this->determineLimits();
		
		std::vector<std::pair<T, T>> limitPairs = pairConsecutiveElementsOf(limits);
					 
		for(size_t i = 0; i < limitPairs.size(); i++){
		
			std::pair<T, T> pair = limitPairs[i];
					 
			Interval<T> currentInterval(
				pair.first,
				true,
				pair.second,
				i == limitPairs.size()-1
			);
			
			intervals.push_back(currentInterval);
		}
		
		return intervals;
	}
					 
private:
	std::vector<T> determineLimits() const{
		
		std::vector<T> limits;
		
		T currentLimit = this->minValue;
		limits.push_back(currentLimit);
		
		T totalDistance = this->maxValue - this->minValue;
		T limitDistance = totalDistance / this->numberOfClasses;					 
		size_t numberOfInnerLimits = this->numberOfClasses -1;

		for(size_t i = 0; i < numberOfInnerLimits; i++){
			currentLimit += limitDistance;
			limits.push_back(currentLimit);
		}
		
		currentLimit = this->maxValue;
		limits.push_back(currentLimit);
		
		return limits;
	}

	static std::vector<std::pair<T, T>> pairConsecutiveElementsOf(std::vector<T> vector){
		
		std::vector<std::pair<T, T>> pairs;
		
		if(vector.size() >= 2){
			
			size_t firstElementIndex;
			size_t secondElementIndex;
			
			for(firstElementIndex = 0; firstElementIndex < vector.size()-1; ++firstElementIndex){
				
				secondElementIndex = firstElementIndex+1;
				
				std::pair<T, T> currentPair(
					vector[firstElementIndex],
					vector[secondElementIndex]
				);
				
				pairs.push_back(currentPair);
			}
		}
		
		return pairs;
	}
};
	

	
} /*namespace histogram*/ } /*namespace BaseLib*/

#endif // HISTOGRAMEQUALSIZEDCLASSINTERVALSFACTORY_H
