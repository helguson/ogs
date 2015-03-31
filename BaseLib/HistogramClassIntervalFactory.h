#ifndef HISTOGRAMCLASSINTERVALFACTORY_H
#define HISTOGRAMCLASSINTERVALFACTORY_H

#include <vector>

#include <string>
#include <sstream>
#include <ostream>

#include "Interval.h"

/**
 * @author Thomas Hennig
 * 
 */

namespace BaseLib{ namespace histogram{


/**
 * assert T implements assignment operator
 */
template<typename T>
class ClassIntervalFactory{
	
public:
	// constructors and deconstructors
	
	// getters and setters
	ClassIntervalFactory<T> & setMinValueTo(T const & value){
		this->minValue = value;
		return *this;
	}
	ClassIntervalFactory<T> & setMaxValueTo(T const & value){
		this->maxValue = value;
		return *this;
	}
	ClassIntervalFactory<T> & setNumberOfClassesTo(size_t const & value){
		this->numberOfClasses = value;
		return *this;
	}
	
	T const & getMinValue() const{
		return this->minValue;
	}
	
	T const & getMaxValue() const{
		return this->maxValue;
	}
	
	size_t const & getNumberOfClasses() const{
		return this->numberOfClasses;
	}
	
	
	// methods
	virtual std::vector<Interval<T>> createClassIntervals() const = 0;
				 
protected:
	T minValue;
	T maxValue;
	size_t numberOfClasses;
	
};


} /*namespace histogram*/ } /*namespace BaseLib*/


#endif // HISTOGRAMCLASSINTERVALFACTORY_H
