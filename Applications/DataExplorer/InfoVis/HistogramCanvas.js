"use strict";

// define infovis package / conglomerate- / accumulation-object
if(window.infovis == undefined){
	window.infovis = {};
}

(function(container){

//#######################
//### HistogramCanvas ###
//#######################
/**
 * uses d3.js, inheritance.js
 * inherits SVGCanvas from SVGCanvas.js
 *
 * attributes
 * - _baseDataAccessor - accessor to bins
 * - _degreeOfInterestList - degree of interest for each DataAccessor in baseDataAccessor
 * - _maximumInterestedValueToMaximumShownValueFactor -
 * - _xAxisHeight -
 * - _yAxisWidth -
 * - _paddingTop -
 * - _paddingLeft -
 * - _paddingBottom -
 * - _paddingRight -
 * - _barsGroup - D3 selection of <g> that contains the bars
 * - _xAxisGroup - D3 selection of <g> that contains elements that form the x axis
 * - _yAxisGroup - D3 selection of <g> that contains elements that form the y axis
 * - _xScale
 * - _heightScale -
 * - _yScale -
 * - _maxCountOfInterest
 * - _maxShownValue - y max
 * - _data
 */

/**
 * @param layoutParameters -
 * 	@property width - target width of SVG node
 * 	@property height - target height of SVG node
 * 	@property xAxisHeight - target height of x axis
 * 	@property yAxisWidth - target width of y axis
 * @param dataParameters
 * 	@property baseDataAccessor - accessor to bins
 * 	@property degreeOfInterestList - degree of interest for each DataAccessor in baseDataAccessor
 */
container.HistogramCanvas = function(parentNode, layoutParameters, dataParameters){

	var instance = container.HistogramCanvas.applyParentConstructorTo(
		this,
		[parentNode]
	);

	instance._setUpDOMStructure();

	instance.setDataParameters(dataParameters);
	instance.setLayoutParameters(layoutParameters);

	this.render();

	return instance;
}
container.HistogramCanvas.extend(infovis.SVGCanvas);

container.HistogramCanvas.prototype._setUpDOMStructure = function(){

	this._barsGroup = this._svgSelection.append("g")
		.attr("class", "bars");
	this._xAxisGroup = this._svgSelection.append("g")
		.attr("class", "x axis");
	this._yAxisGroup = this._svgSelection.append("g")
		.attr("class", "y axis");
}

// @param dataParameters
//	@property baseDataAccessor - accessor to bins
//	@property degreeOfInterestList - degree of interest for each DataAccessor in baseDataAccessor
container.HistogramCanvas.prototype.setDataParameters = function(dataParameters){

	this._baseDataAccessor = infovis.firstExisting([dataParameters, 'baseDataAccessor'], this._baseDataAccessor);
	this._degreeOfInterestList = infovis.firstExisting([dataParameters, 'degreeOfInterestList'], this._degreeOfInterestList);

	if(this._baseDataAccessor.length != this._degreeOfInterestList.length){
		throw new Error("number of degrees of interest do not equal number of bins");
	}
}

container.HistogramCanvas.prototype.setLayoutParameters = function(layoutParameters){

	// call parent setLayoutParameters
	var parentPrototype = container.HistogramCanvas.prototype.__proto__;
	parentPrototype.setLayoutParameters.call(this, layoutParameters);

	var standardLayoutParameters = {
		maximumInterestedValueToMaximumShownValueFactor: 1.1,
		xAxisHeight: 25,
		yAxisWidth: 50,
		paddingTop: 10,
		paddingLeft: 10,
		paddingBottom: 10,
		paddingRight: 25
	}

	this._maximumInterestedValueToMaximumShownValueFactor = infovis.firstExisting(
		[layoutParameters, "maximumInterestedValueToMaximumShownValueFactor"],
		[this, "_maximumInterestedValueToMaximumShownValueFactor"],
		standardLayoutParameters["maximumInterestedValueToMaximumShownValueFactor"]
	);
	this._xAxisHeight = infovis.firstExisting(
		[layoutParameters, "xAxisHeight"],
		[this, "_xAxisHeight"],
		standardLayoutParameters["xAxisHeight"]
	);
	this._yAxisWidth = infovis.firstExisting(
		[layoutParameters, "yAxisWidth"],
		[this, "_yAxisWidth"],
		standardLayoutParameters["yAxisWidth"]
	);

	this._paddingTop = infovis.firstExisting(
		[layoutParameters, "paddingTop"],
		[this, "_paddingTop"],
		standardLayoutParameters["paddingTop"]
	);
	this._paddingLeft = infovis.firstExisting(
		[layoutParameters, "paddingLeft"],
		[this, "_paddingLeft"],
		standardLayoutParameters["paddingLeft"]
	);
	this._paddingBottom = infovis.firstExisting(
		[layoutParameters, "paddingBottom"],
		[this, "_paddingBottom"],
		standardLayoutParameters["paddingBottom"]
	);
	this._paddingRight = infovis.firstExisting(
		[layoutParameters, "paddingRight"],
		[this, "_paddingRight"],
		standardLayoutParameters["paddingRight"]
	);
}

container.HistogramCanvas.prototype._renderBars = function(){

	//#######################
	//### private methods ###
	//#######################
	// @param this - barSelection
	// @param canvasHeight - height of Area for bars
	var setGeometricalAttributes = function(xScale, yScale, heightScale, rectangleWidth){

		this
			.attr(
				"x",
				function(datum, index){
					return xScale(index);
				}
			)
			.attr(
				"y",
				function(datum, index){
					var count = datum[0].access();

					return yScale(count);
				}
			)
			.attr(
				"width",
				rectangleWidth
			)
			.attr(
				"height",
				function(datum, index){
					var count = datum[0].access();

					return heightScale(count);
				}
			);
	}

	// set class by DegreeofInterest
	// @param this - barSelection
	var setClassAttributes = function(maxShownValue){
		this
			.attr(
				"class",
				function(datum, index){
					var result;

					var datumAccessor = datum[0];
					var degreeOfInterest = datum[1];

					if(degreeOfInterest == 1){
						result = "barOfInterest";
					}
					else{
						var count = datumAccessor.access();

						if(count > maxShownValue){
							result = "partiallyShownContextBar";
						}
						else{
							result = "contextBar";
						}
					}

					return result;
				}
			);
	}

	//############
	//### main ###
	//############


	var barSelection = this._barsGroup.selectAll("rect");

	barSelection = barSelection.data(this._data);

	barSelection.exit()
		.remove();

	var svg = this._svgSelection.node;
	barSelection.enter()
		.append("rect");

	var rectangleWidth = this._xScale.rangeBand();
	setGeometricalAttributes.call(
		barSelection,
		this._xScale,
		this._yScale,
		this._heightScale,
		rectangleWidth
	);

	setClassAttributes.call(
		barSelection,
		this._maxShownValue
	);
}

container.HistogramCanvas.prototype._setUpGroupAttributes = function(){
	//#######################
	//### private methods ###
	//#######################
	var applyTranslation = function(selection, dimension){

		selection.attr(
			"transform",
			"translate(" + dimension.x.toString() + "," + dimension.y.toString() + ")"
		);
	}

	//############
	//### main ###
	//###########

	applyTranslation(
		this._barsGroup,
		this._getDimensionsOfBarsGroup()
	);

	applyTranslation(
		this._xAxisGroup,
		this._getDimensionsOfXAxisGroup()
	);

	applyTranslation(
		this._yAxisGroup,
		this._getDimensionsOfYAxisGroup()
	);
}

container.HistogramCanvas.prototype._getDimensionsOfBarsGroup = function(){

	return {
		x: this._paddingLeft + this._yAxisWidth,
		y: this._paddingTop,
		width: this._width -(this._yAxisWidth + this._paddingLeft + this._paddingRight),
		height: this._height - (this._xAxisHeight + this._paddingTop + this._paddingBottom)
	};
}

container.HistogramCanvas.prototype._getDimensionsOfXAxisGroup = function(){

	var barsDimensions = this._getDimensionsOfBarsGroup();

	return {
		x: barsDimensions.x,
		y: barsDimensions.y + barsDimensions.height,
		width: barsDimensions.width,
		height: this._xAxisHeight
	};
}

container.HistogramCanvas.prototype._getDimensionsOfYAxisGroup = function(){

	var barsDimensions = this._getDimensionsOfBarsGroup();

	return {
		x: barsDimensions.x,
		y: barsDimensions.y,
		width: this._yAxisWidth,
		height: barsDimensions.height
	};
}

container.HistogramCanvas.prototype._prepareData = function(){
	//#######################
	//### private methods ###
	//#######################
	var pairs = function(a, b){
		if(a.length != b.length){
			throw new Error("cannot combine arrays of unequal length");
		}

		var result = [];

		for(var i = 0; i < a.length; i++){
			result.push(
				[a[i], b[i]]
			);
		}

		return result;
	}

	//############
	//### main ###
	//############

	this._data = pairs(this._baseDataAccessor.split() ,this._degreeOfInterestList);
}

container.HistogramCanvas.prototype._setUpScales = function(){

	var dimensions = this._getDimensionsOfBarsGroup();

	this._xScale = d3.scale.ordinal()
		.domain(
			this._data.map(
				function(element, index){
					return index;
				}
			)
		)
		.rangeBands([0, dimensions.width], 0.0, 0.05);


	this._heightScale = d3.scale.linear()
		.domain([0, this._maxShownValue])
		.range([0, dimensions.height]);

	this._yScale = d3.scale.linear()
		.domain([0, this._maxShownValue])
		.range([dimensions.height,0]);

	var oldTicks = this._yScale.ticks;
	this._yScale.ticks = function(n){

		var ticks = oldTicks.call(this, n);

		var filteredTicks = ticks.filter(
			function(tick){
				return tick == Math.floor(tick);
			}
		);

		return filteredTicks;
	}
}

container.HistogramCanvas.prototype._determineMaxCountOfInterest = function(){

	var countsOfInterest = this._data
		.filter(
			function(e){
				var degreeOfInterest = e[1];
				var elementIsOfInterest = degreeOfInterest == 1;

				return elementIsOfInterest;
			}
		)
		.map(
			function(e){
				return e[0].access();
			}
		);

	return Math.max.apply(Math, countsOfInterest);
}

container.HistogramCanvas.prototype._determineMaxShownValue = function(maxCountOfInterest){

	return maxCountOfInterest * this._maximumInterestedValueToMaximumShownValueFactor;
}

container.HistogramCanvas.prototype._determineAdditionalInformation = function(){

	this._prepareData();

	// determine some needed meta data
	this._maxCountOfInterest = this._determineMaxCountOfInterest();
	this._maxShownValue = this._determineMaxShownValue(this._maxCountOfInterest);

	this._setUpScales();
}

container.HistogramCanvas.prototype._renderXAxis = function(){
	//#######################
	//### private methods ###
	//#######################
	// based on "Datenmodell_Histogrammdaten.png/graphml"
	var extractIntervalFrom = function(datumAccessor){

		return {
			lowerLimit: datumAccessor.meta("lower_limit").access(),
			upperLimit: datumAccessor.meta("upper_limit").access()
		};
	}

	var extractIntervalsFrom = function(baseDataAccessor){

		return baseDataAccessor.split().map(extractIntervalFrom);
	}


	//############
	//### main ###
	//############
	var intervals = extractIntervalsFrom(this._baseDataAccessor);

	// assert intervals are sorted
	var limits = intervals.map(
		function(i){
			return i.lowerLimit;
		}
	);
	limits.push( intervals[intervals.length-1].upperLimit );

	// create scale
	var d = this._getDimensionsOfXAxisGroup();
	var limitScale = d3.scale.ordinal()
		.domain(limits)
		.rangePoints([0, this._getDimensionsOfXAxisGroup().width],0.05);



	/**
	 * @param n - target number of ticks
	 */
	limitScale.ticks = function(n){

		var domain = this.domain();
		var ticks;

		if(n < domain.length){

			ticks = [];

			ticks.push(domain[0]);

			// determine interticks
			for(var i = 1; i <= n-2; i++){

				var alpha = i/(n-1);
				// in [0, 1]

				var rawDomainIndex = alpha*(domain.length-1);
				// in [0, domain.length-1]

				var domainIndex = Math.floor(rawDomainIndex);

				ticks.push(domain[domainIndex]);
			}

			ticks.push(domain[domain.length-1]);
		}
		else{
			ticks = domain;
		}

		return ticks;
	}

	// assert limits are sorted in ascending order
	var range = [limits[0], limits[limits.length-1]];

	var formatter = new container.HistogramCanvas.Formatter(range);

	limitScale.tickFormat = function(){
		return formatter.getBoundFormatFunction();
	};

	// create axis
	var xAxis = d3.svg.axis()
		.scale(limitScale)
		.orient("bottom");

	this._xAxisGroup.call(xAxis);
}

container.HistogramCanvas.prototype._renderYAxis = function(){

	// get y scale
	var yAxis = d3.svg.axis()
		.scale(this._yScale)
		.orient("left");

	this._yAxisGroup.call(yAxis);
}

container.HistogramCanvas.prototype.render = function(){

	this._determineAdditionalInformation();

	this._setUpGroupAttributes();

	this._renderBars();
	this._renderYAxis();
	this._renderXAxis();
}

//##########################
//### ScaleTickFormatter ###
//##########################
// assert magnitude of neighbourwise differences is similar to magnitude of
// difference between minimum and maximum

// detemines information part of given number with greater or equal magnitude
var g = function(value, magnitude){

	var round = value > 0? Math.floor: Math.ceil;

	var c = Math.pow(10, magnitude);

	return round(value/c)*c;
}

// determines information part with smaller magnitude
var f = function(value, magnitude){

	return value - g(value, magnitude);
}

var magnitudeOf = function(value){

	var abs = Math.abs;
	var floor = Math.floor;
	var log10 = function(v){

		return Math.log(v)/Math.log(10);
	}

	return floor(
		log10(
			abs(value)
		)
	);
}


container.HistogramCanvas.Formatter = function(range){

	this._determineFormatByAnalysing(range);

	return this;
}

container.HistogramCanvas.Formatter.prototype._determineFormatByAnalysing = function(range){


	var magnitudeOfDifference = magnitudeOf(range[1]-range[0]);

	this._formatObject = new container.HistogramCanvas.Formatter.DecimalFormat(2-magnitudeOfDifference);
}

container.HistogramCanvas.Formatter.prototype.format = function(value){

	return this._formatObject.format(value);
}

container.HistogramCanvas.Formatter.prototype.getBoundFormatFunction = function(){

	var self = this;

	return function(){

		return self.format.apply(self, arguments);
	};
}

// @param numberOfSignificantFigures - number of "digits that carry meaning contributing to [a numbers] precision" [wikipedia: significant_figures]
container.HistogramCanvas.Formatter.DecimalFormat = function(numberOfSignificantFigures){

	this._numberOfSignificantFigures = numberOfSignificantFigures;
}

container.HistogramCanvas.Formatter.DecimalFormat.prototype.format = function(value){

	var round = Math.round;
	var c = Math.pow(10, this._numberOfSignificantFigures);

	var roundedValue = round(value*c)/c;

	return roundedValue.toString();
}

//#########################
//### HoverDetailHelper ###
//#########################

})(infovis);
