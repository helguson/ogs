#ifndef HISTOGRAMCLASSINTERVALFACTORY_H
#define HISTOGRAMCLASSINTERVALFACTORY_H

#include <vector>

#include "Interval.h"


namespace BaseLib{ namespace histogram{


/**
 * @author Thomas Hennig
 * @brief interface for interval generation for \c BaseLib::Histogram
 * 
 * assert T implements assignment operator
 */
template<typename T>
class ClassIntervalFactory{
	
public:
	// constructors and deconstructors
	
	// getters and setters
	virtual ClassIntervalFactory<T> & setMinValueTo(T const & value) = 0;
	
	virtual ClassIntervalFactory<T> & setMaxValueTo(T const & value) = 0;
	
	virtual ClassIntervalFactory<T> & setNumberOfClassesTo(size_t const & value) = 0;
	
	virtual T const & getMinValue() const = 0;
	
	virtual T const & getMaxValue() const = 0;
	
	virtual size_t const & getNumberOfClasses() const = 0;
	
	// methods
	/** 
	 * @brief creates a set of pairwise disjunct intervals with numberOfClasses 
	 * elements, whose union equals interval [minValue, maxValue]
	 * may contain empty intervals: (a, a)
	 */
	virtual std::vector<Interval<T>> createClassIntervals() const = 0;
				 	
};


} /*namespace histogram*/ } /*namespace BaseLib*/


#endif // HISTOGRAMCLASSINTERVALFACTORY_H
