#ifndef HISTOGRAMABSTRACTCLASSINTERVALFACTORY_H
#define HISTOGRAMABSTRACTCLASSINTERVALFACTORY_H


#include <utility>
#include "HistogramClassIntervalFactory.h"


namespace BaseLib{ namespace histogram{


/**
 * @author Thomas Hennig
 * @brief implementation of \c BaseLib::histogram::ClassIntervalFactory that
 * introduces a new abstract method to generate interval limits
 */
template<typename T>
class AbstractClassIntervalFactory: public ClassIntervalFactory<T>{

public: 
	// constructors and deconstructors
	
	// getters and setters
	virtual ClassIntervalFactory<T> & setMinValueTo(T const & value){
		this->minValue = value;
		return *this;
	}
	virtual ClassIntervalFactory<T> & setMaxValueTo(T const & value){
		this->maxValue = value;
		return *this;
	}
	virtual ClassIntervalFactory<T> & setNumberOfClassesTo(size_t const & value){
		this->numberOfClasses = value;
		return *this;
	}

	virtual T const & getMinValue() const{
		return this->minValue;
	}
	
	virtual T const & getMaxValue() const{
		return this->maxValue;
	}
	
	virtual size_t const & getNumberOfClasses() const{
		return this->numberOfClasses;
	}
	
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
	
protected:
	// attributes
	T minValue;
	T maxValue;
	size_t numberOfClasses;
	
	// abstract methods
	virtual std::vector<T> determineLimits() const = 0;
	
	
	// static methods
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


#endif // HISTOGRAMABSTRACTCLASSINTERVALFACTORY_H
