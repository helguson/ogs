#ifndef HISTOGRAMDATATRANSFORMER_H
#define HISTOGRAMDATATRANSFORMER_H

#include <memory>
#include <vector>

#include <QVariant>
#include <QString>

#include "BaseLib/Histogram.h"
#include "DataBuilder.h"
#include "BaseLib/Interval.h"

template<typename T>
class HistogramDataTransformer{
	
public:
	// constants
	const QString IS_INCLUSIVE_IDENTIFIER = QString("is_inclusive");
	const QString LOWER_LIMIT_IDENTIFIER = QString("lower_limit");
	const QString UPPER_LIMIT_IDENTIFIER = QString("upper_limit");
	
	// constructors and destructors
	HistogramDataTransformer()
	:dataBuilder(), baseDataIndices(){
	}

	// getters and setters
	std::unique_ptr<QVariantList> getValues() const{
		return this->dataBuilder.getValues();
	}
	std::unique_ptr<QVariantList> getMetaDataRelation() const{
		return this->dataBuilder.getMetaDataRelation();
	}
	std::unique_ptr<QVariantList> getBaseDataIndices() const{
		
		return std::unique_ptr<QVariantList>(
			new QVariantList(DataBuilder::toQVariantList(this->baseDataIndices))
		);
	}
	
	// methods
	void process(BaseLib::Histogram<T> const & histogram){
		
		this->reset();
		
		this->setUp();
		
		std::vector<BaseLib::Interval<T>> const & intervals = histogram.getIntervals();
		std::vector<size_t> const & counts = histogram.getBinCounts();
		
		for(size_t i = 0; i < intervals.size(); i++){
			
			int binDateIndex = this->processBin(
				counts[i],
				intervals[i]
			);
			
			this->baseDataIndices.push_back(binDateIndex);
		}
	}
	
	
private:
	// attributes
	DataBuilder dataBuilder;
	QList<int> baseDataIndices;
	
	int isInclusiveIdentifierDateIndex;
	int upperLimitIdentifierDateIndex;
	int lowerLimitIdentifierDateIndex;
	
	// methods
	void reset(){
		this->dataBuilder.reset();
		
		this->baseDataIndices.clear();
		
		
	}
	
	void setUp(){
		
		this->isInclusiveIdentifierDateIndex = this->dataBuilder.addDateAndReturnIndex(
			QVariant(IS_INCLUSIVE_IDENTIFIER)		       
		);
		
		this->upperLimitIdentifierDateIndex = this->dataBuilder.addDateAndReturnIndex(
			QVariant(UPPER_LIMIT_IDENTIFIER)		       
		);
		
		this->lowerLimitIdentifierDateIndex = this->dataBuilder.addDateAndReturnIndex(
			QVariant(LOWER_LIMIT_IDENTIFIER)		       
		);
	}
	
	std::pair<int, int> buildInterval(BaseLib::Interval<T> const & interval){
		
		int lowerLimitDateIndex = this->buildIntervalLimit(
			interval.getLowerLimit(),
			true,
			interval.isLowerLimitInclusive()
		);
		int upperLimitDateIndex = this->buildIntervalLimit(
			interval.getUpperLimit(),
			false,
			interval.isUpperLimitInclusive()
		);
		
		return std::pair<int,int>(lowerLimitDateIndex, upperLimitDateIndex);
	}
	
	int buildIntervalLimit(T const & value, bool isLowerLimit, bool isInclusive){
		
		int isInclusiveDateIndex = this->dataBuilder.addDateAndReturnIndex(
			QVariant(isInclusive)
		);
		
		this->dataBuilder.addAsMetaDataFor(
			isInclusiveDateIndex,
			this->isInclusiveIdentifierDateIndex
		);
		
		int valueDateIndex = this->dataBuilder.addDateAndReturnIndex(
			QVariant(value)
		);
		
		this->dataBuilder.addAsMetaDataFor(
			valueDateIndex,
			(isLowerLimit)? this->lowerLimitIdentifierDateIndex : this->upperLimitIdentifierDateIndex
		);
		
		this->dataBuilder.addAsMetaDataFor(
			valueDateIndex,
			isInclusiveDateIndex
		);
		
		return valueDateIndex;
	}
	
	int processBin(size_t const & count, BaseLib::Interval<T> const & interval){
		
		std::pair<int, int> limitDateIndices = this->buildInterval(interval);
		
		int countDateIndex = this->dataBuilder.addDateAndReturnIndex(
			QVariant((uint) count)
		);
		
		this->dataBuilder.addAsMetaDataFor(
			countDateIndex,
			limitDateIndices.first	// lowerLimit
		);
		
		this->dataBuilder.addAsMetaDataFor(
			countDateIndex,
			limitDateIndices.second	// lowerLimit
		);
		
		return countDateIndex;
	}
};

#endif // HISTOGRAMDATATRANSFORMER_H
