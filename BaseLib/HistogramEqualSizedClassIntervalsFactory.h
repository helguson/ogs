#ifndef HISTOGRAMEQUALSIZEDCLASSINTERVALSFACTORY_H
#define HISTOGRAMEQUALSIZEDCLASSINTERVALSFACTORY_H

#include <utility>

#include "HistogramClassIntervalFactory.h"

namespace BaseLib { namespace histogram {



/**
 * @author Thomas Hennig
 * @brief implementation of \c histogram::ClassIntervalFactory that creates a set
 * of equal sized intervals
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
			
			bool lowerLimitIsInclusive;
			bool upperLimitIsInclusive;
			
			if(pair.first != pair.second){ // normal case
				lowerLimitIsInclusive = true;
			}
			else{
				if(i < limitPairs.size()-1){ // has next
					lowerLimitIsInclusive = false;
				}
				else{
					lowerLimitIsInclusive = true;
				}
			}
			
			upperLimitIsInclusive = (i == limitPairs.size()-1);
					 
			Interval<T> currentInterval(
				pair.first,
				lowerLimitIsInclusive,
				pair.second,
				upperLimitIsInclusive
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
