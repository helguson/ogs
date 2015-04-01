/**
 * \file
 * \author Dmitrij Naumov
 * \brief Implementation of Histogram class.
 *
 * \copyright
 * Copyright (c) 2012-2015, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 */

#ifndef BASELIB_HISTOGRAM_H
#define BASELIB_HISTOGRAM_H

#include <algorithm>
#include <functional>
#include <cmath>
#include <iterator>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "HistogramClassIntervalFactory.h"
#include "HistogramEqualSizedClassIntervalsFactory.h"
#include "Interval.h"
// ThirdParty/logog
#include "logog/include/logog.hpp"


namespace BaseLib
{
/** Basic Histogram implementation.
 *
 * Creates histogram from input data of type \c T.
 */
template <typename T>
class Histogram
{
public:
	typedef typename std::vector<T> Data; /// Underlying input data vector type.

	typedef typename histogram::ClassIntervalFactory<T> FactoryInterface;
	typedef typename std::unique_ptr<FactoryInterface> FactoryPtr;
	typedef	typename histogram::EqualSizedClassIntervalsFactory<T> DefaultFactory;
	typedef typename std::unique_ptr<DefaultFactory> DefaultFactoryPtr;
	
public:
	/** Creates histogram of the given element in the range \c [first, last).
	 *
	 * Input data is copied into \c std::vector.
	 *
	 * \param data Range of elements to create histogram from.
	 * \param nr_bins Number of bins in histogram.
	 * \param computeHistogram Compute histogram if set. If not set user must
	 *        call \c update() before accessing data.
	 * \param classIntervalFactory - factory object for creation of intervals
	 */
	template <typename InputIterator>
	Histogram(InputIterator first, InputIterator last, size_t const & nr_bins = 16,
	          const bool computeHistogram = true,
		  FactoryPtr classIntervalFactory = std::move(DefaultFactoryPtr(new DefaultFactory())))
	: _data(first, last), classIntervalFactory(std::move(classIntervalFactory))
	{
		init(computeHistogram, nr_bins);
	}

	/** Creates histogram from \c std::vector.
	 * \param data Input vector.
	 * \param nr_bins Number of bins in histogram.
	 * \param computeHistogram Compute histogram if set. If not set user must call
	 * \c update() before accessing data.
	 * \param classIntervalFactory - factory object for creation of intervals
	 */
	Histogram(std::vector<T> const& data, size_t const & nr_bins = 16,
	          const bool computeHistogram = true,
		  FactoryPtr classIntervalFactory = std::move(DefaultFactoryPtr(new DefaultFactory())))
	: _data(data), classIntervalFactory(std::move(classIntervalFactory))
	{
		init(computeHistogram, nr_bins);
	}

	/** Updates histogram using sorted \c _data vector.
	 *
	 * Start histogram creation with first element. Then find first element in
	 * the next histogram bin. Number of elments in the bin is the difference
	 * between these two iterators.
	 * \verbatim
	   [0.1, 0.2, ..., 0.7 , ..., 0.7+binWidth = 0.9,  1.0  , ..., last]
	                   it             itEnd - 1      itEnd
	   \endverbatim
	 */
	void update()
	{
		if (!_dirty)
			return;
		
		// determine intervals
		this->classIntervals = this->classIntervalFactory->createClassIntervals();

		// allocate enough bins
		_histogram.resize(this->classIntervals.size());
		
		this->determineHistogram();
		
		_dirty = false;
	}

	void setMinimum(T const & value) { this->classIntervalFactory->setMinValueTo(value); _dirty = true; }
	void setMaximum(T const & value) { this->classIntervalFactory->setMaxValueTo(value); _dirty = true; }
	void setNrBins(size_t const & value){ this->classIntervalFactory->setNumberOfClassesTo(value); _dirty = true; }

	const Data& getSortedData() const { return _data; }
	const std::vector<std::size_t>& getBinCounts() const { return _histogram; }
	size_t const & getNrBins() const { return this->classIntervalFactory->getNumberOfClasses(); }
	const T& getMinimum() const { return this->classIntervalFactory->getMinValue(); }
	const T& getMaximum() const { return this->classIntervalFactory->getMaxValue(); }
	std::vector<T> const getBinWidth() const {
		std::vector<T> result;
		
		for(Interval<T> currentInterval: this->classIntervals){
			
			result.push_back(currentInterval.getWidth());
		}
		
		return result;
	}

	void
	prettyPrint(std::ostream& os, const unsigned int line_width = 16) const
	{
		const std::size_t count_max =
		        *std::max_element(_histogram.begin(), _histogram.end());
		
		for (unsigned int bin = 0; bin < _histogram.size(); ++bin)
		{
			os << this->classIntervals[bin];
			      
			os << "\t" << _histogram[bin] << "\t";

			const int n_stars =
			        std::ceil(line_width * ((double)_histogram[bin] / count_max));
			for (int star = 0; star < n_stars; star++)
				os << "*";
			os << "\n";
		}
	}

	int write(std::string const& file_name, std::string const& data_set_name, std::string const& param_name) const
	{
		if (file_name.empty())
		{
			ERR ("No file name specified.");
			return 1;
		}

		std::ofstream out (file_name);
		if (!out)
		{
			ERR("Error writing histogram: Could not open file.");
			return 1;
		}

		out << "# Histogram for parameter " << param_name << " of data set " << data_set_name << "\n";
		std::size_t const n_bins = this->getNrBins();
		std::vector<size_t> const& bin_cnts(this->getBinCounts());
		double const min (this->getMinimum());
		std::vector<double> const bin_width (this->getBinWidth());

		for (size_t k(0); k < n_bins; k++)
			out << min+k*bin_width[k] << " " << bin_cnts[k] << "\n";
		out.close ();
		return 0;
	}
	
	/**
	 * @brief determines iterator to first element that satisfies a given predicate
	 * @param first - iterator to first element that is under test
	 * @param last - iterator to first element after last element that is
	 * under test
	 * @param predicate - TODO
	 * assert for every given predicate P: there is an element E in [first, last)
	 * with every element Eb before E: Eb does not satisfy P; every element Ea 
	 * after and including E: Ea satisfies P
	 * @return iterator to first element that is considered greater or equal
	 * \c value or \c last if no element between \c first and \c last
	 * satisfies the criteria
	 * 
	 * 
	 * modification of http://www.cplusplus.com/reference/algorithm/upper_bound/
	 */
	template <typename ForwardIterator>
	static ForwardIterator getIteratorToNextElementSatisfyingPredicate(ForwardIterator first, ForwardIterator last, std::function<bool(T)> predicate){
		
		using DifferenceType = typename std::iterator_traits<ForwardIterator>::difference_type;
		
		ForwardIterator it;
		DifferenceType count, step;
		
		count = std::distance(first,last);
		
		while (count>0)
		{
			it = first;
			step=count/2;
			std::advance(it,step);
			
			if (!(predicate(*it))){
				it++;
				first=it;
				count-=step+1;
			}
			else count=step;
		}
		return first;
	}
	
	

protected:
	/** Initialize class members after constructor call.
	 */
	void init(const bool computeHistogram, size_t const & nr_bins)
	{
		std::sort(_data.begin(), _data.end());
		
		this->setMinimum(_data.front());
		this->setMaximum(_data.back());
		this->setNrBins(nr_bins);
		
		if (computeHistogram)
			update();
	}
	
	// assert valid class intervals
	//	every element between min and max is contained in exactly one interval
	void determineHistogram(){
		
		typedef typename Data::const_iterator DataCI;
		typedef typename std::function<bool(T)> Predicate_T;
		
		Interval<T> firstInterval = this->classIntervals.front();
		Predicate_T firstValidElementPredicate =
			[firstInterval](T const & value)
			{
				return !(value < firstInterval);
			};
		
		// skipping elements that are not contained in first interval
		DataCI it = getIteratorToNextElementSatisfyingPredicate<DataCI>(
			(DataCI) _data.begin(),
			(DataCI) _data.end(),
			firstValidElementPredicate
		);
		DataCI itEnd;
		
		
		for (size_t bin = 0; bin < this->classIntervalFactory->getNumberOfClasses(); bin++)
		{		
			Interval<T> currentInterval = this->classIntervals[bin];
			Predicate_T notContainedInCurrentIntervalPredicate =
				[currentInterval](T const & value)
				{
					return !currentInterval.contains(value);
				};
			
			itEnd = getIteratorToNextElementSatisfyingPredicate<DataCI>(
					it,
					(DataCI)_data.end(),
			                notContainedInCurrentIntervalPredicate
			);
			
			_histogram[bin] = std::distance(it, itEnd);
			
			it = itEnd;
		}
		
		// elements greater than last interval are skipped
	}

protected:
	Data _data;
	std::vector<size_t> _histogram;
	
	FactoryPtr classIntervalFactory;
	std::vector<Interval<T>> classIntervals;

private:
	bool _dirty; ///< When set \c update() will recompute histogram.
};

/** Writes histogram to output stream.
 *
 * Writes histogram properties in this order:
 * number of bins, minimum, maximum, bin0 count, ..., binN-1 count.
 */
template <typename T>
std::ostream&
operator<<(std::ostream& os, const Histogram<T>& h)
{
	os << h.getNrBins() << " "
	   << h.getMinimum() << " "
	   << h.getMaximum() << " ";
	std::copy(h.getBinCounts().begin(), h.getBinCounts().end(),
	          std::ostream_iterator<T>(os, " "));
	return os << std::endl;
}
}   // namespace BaseLib

#endif  // BASELIB_HISTOGRAM_H
