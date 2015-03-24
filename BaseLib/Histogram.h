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
#include <ostream>
#include <vector>

namespace BaseLib
{
/** Basic Histogram implementation.
 *
 * Creates histogram from input data of type \c T counting elements for intervals
 * [min, b1),[b1, b2), ... ,[bn-1, bn),[bn, max]
 */
template <typename T>
class Histogram
{
public:
	typedef typename std::vector<T> Data; ///< Underlying input data vector
	                                      ///  type.

public:
	/** Creates histogram of the given element in the range \c [first, last).
	 *
	 * Input data is copied into \c std::vector.
	 *
	 * \param data Range of elements to create histogram from.
	 * \param nr_bins Number of bins in histogram.
	 * \param computeHistogram Compute histogram if set. If not set user must
	 *        call \c update() before accessing data.
	 */
	template <typename InputIterator>
	Histogram(InputIterator first, InputIterator last, const int nr_bins = 16,
	          const bool computeHistogram = true )
		: _data(first, last), _nr_bins(nr_bins)
	{
		init(computeHistogram);
	}

	/** Creates histogram from \c std::vector.
	 * \param data Input vector.
	 * \param nr_bins Number of bins in histogram.
	 * \param computeHistogram Compute histogram if set. If not set user must call
	 * \c update() before accessing data.
	 */
	Histogram(std::vector<T> const& data, const unsigned int nr_bins = 16,
	          const bool computeHistogram = true)
		: _data(data), _nr_bins(nr_bins)
	{
		init(computeHistogram);
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
	void
	update()
	{
		if (!_dirty)
			return;

		_bin_width = (_max - _min) / _nr_bins;

		typedef typename Data::const_iterator DataCI;
		DataCI it = _data.begin();
		DataCI itEnd;
		for (unsigned int bin = 0; bin < _nr_bins; bin++)
		{
			
			itEnd = getIteratorToNextGreaterOrEqualElement<DataCI>(
					it,
					(DataCI)_data.end(),
			                _min + (bin + 1) * _bin_width
			);
			
			_histogram[bin] = std::distance(it, itEnd);
			
			it = itEnd;
		}
		
		_histogram[_nr_bins-1] += std::distance(it, (DataCI)_data.end());
		
		
		_dirty = false;
	}

	void setMinimum(const T& minimum) { _min = minimum; _dirty = true; }
	void setMaximum(const T& maximum) { _max = maximum; _dirty = true; }

	const Data& getSortedData() const { return _data; }
	const std::vector<std::size_t>& getBinCounts() const { return _histogram; }
	const unsigned int& getNrBins() const { return _nr_bins; }
	const T& getMinimum() const { return _min; }
	const T& getMaximum() const { return _max; }
	const T& getBinWidth() const { return _bin_width; }

	void
	prettyPrint(std::ostream& os, const unsigned int line_width = 16) const
	{
		const std::size_t count_max =
		        *std::max_element(_histogram.begin(), _histogram.end());
		for (unsigned int bin = 0; bin < _nr_bins; ++bin)
		{
			os << "[" << _min + bin * _bin_width << ", " << _min +
			        (bin + 1) * _bin_width << ((bin==_nr_bins-1)?"]\t":")\t");
			os << _histogram[bin] << "\t";

			const int n_stars =
			        std::ceil(line_width * ((double)_histogram[bin] / count_max));
			for (int star = 0; star < n_stars; star++)
				os << "*";
			os << "\n";
		}
	}

protected:
	/** Initialize class members after constructor call.
	 */
	void init(const bool computeHistogram = true)
	{
		std::sort(_data.begin(), _data.end());
		_histogram.resize(_nr_bins);
		_min = _data.front();
		_max = _data.back();
		_bin_width = (_max - _min) / _nr_bins;

		_dirty = true;
		if (computeHistogram)
			update();
	}

protected:
	Data _data;
	const unsigned int _nr_bins;
	std::vector<std::size_t> _histogram;
	T _min, _max; ///< Minimum and maximum input data values.
	T _bin_width;

private:
	bool _dirty; ///< When set \c update() will recompute histogram.
	
	
	/**
	 * @brief determines iterator to first element that is greater or equal
	 * to a given value
	 * @param first - iterator to first element that is under test
	 * @param last - iterator to first element after last element that is
	 * under test
	 * @param value - renference value for comparison 
	 * @return iterator to first element that is considered greater or equal
	 * \c value or \c last if no element between \c first and \c last
	 * satisfies the criteria
	 * 
	 * modification of http://www.cplusplus.com/reference/algorithm/upper_bound/
	 */
	template <typename ForwardIterator>
	static ForwardIterator getIteratorToNextGreaterOrEqualElement(ForwardIterator first, ForwardIterator last, T const & value){
		
		ForwardIterator it;
		typename std::iterator_traits<ForwardIterator>::difference_type count, step;
		
		count = std::distance(first,last);
		
		while (count>0)
		{
			it = first;
			step=count/2;
			std::advance(it,step);
			
			if (!(typename std::greater_equal<T>::greater_equal()(*it,value))){
				it++;
				first=it;
				count-=step+1;
			}
			else count=step;
		}
		return first;
	}
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
