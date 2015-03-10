// in the following "DO" describes an element of a DOM tree

// @param containerHtmlId - HTML-ID
RickshawFacade = function(containerHtmlId){

	// DOM tree elements
	this._containerDO	= document.getElementById(containerHtmlId);
	this._yAxisDO		= undefined;
	this._chartDO		= undefined;
	this._rangeSliderDO	= undefined;
	this._legendDO		= undefined;
	// rickshaw objects
	this._graph		= undefined;
	this._xAxis		= undefined;
	this._yAxis		= undefined;
	this._rangeSlider	= undefined;
	this._legend		= undefined;
	// other
	this._seriesData	= undefined;

	return this;
}

RickshawFacade.Y_AXIS_DO_CLASS = "y_axis";
RickshawFacade.CHART_DO_CLASS = "chart";
RickshawFacade.RANGESLIDER_DO_CLASS = "range_slider";
RickshawFacade.CHART_AND_SLIDER_DO_CLASS = "chart_and_slider_group";
RickshawFacade.LEGEND_DO_CLASS = "legend";

/**
 * @param seriesData = [
 * 	{
 * 		name
 * 		xAttribute - identifier for x attribute
 * 		yAttribute - identifier for y attribute
 * 		sortingAttribute - identifier for sorting attribute
 * 		data: {
 * 			baseAccessor,
 * 			categoryAccessorFunctorFactory(exampleElementAccessor, category)
 * 				@returns elementCategoryAccessor(elementAccessor)
 * 					@param elementAccessor - rowAccessor in case of DSV
 * 			typeFunctor(elementCategoryAccessor)
 * 			unitFunctor(elementCategoryAccessor)
 * 			valueFunctor(elementCategoryAccessor)
 * 		}
 * 	}
 * ]
 */
RickshawFacade.prototype.setUp = function(seriesData){
	//#######################
	//### private methods ###
	//#######################

	// @param this - current rickshawFacade
	var setUpAxes = function(){

		var info = extractAxesInformation.call(this);

		setUpXAxis.call(this, info.xAttributeInformation);
		setUpYAxis.call(this, info.yAttributeInformation);
	}

	/**
	 * @param this - current rickshawFacade
	 * @param categoryAccessorFunctorFactory(elementAccessor, category)
	 * 	@returns function that returns on application to elementAccessor or equal structured accessor an accessor to category
	 * @returns { type:.. , unit:.. }
	 */
	var extractCategoryInformation = function(dataAccessor, category, categoryAccessorFunctorFactory, unitFunctor, typeFunctor){

		// assert first element is representative for all other elements
		var exampleElementAccessor = dataAccessor.at(0);

		var categoryAccessor = categoryAccessorFunctorFactory(
			exampleElementAccessor,
			category
		)(exampleElementAccessor);

		var type = typeFunctor(categoryAccessor).access();
		var unit = unitFunctor(categoryAccessor).access();

		return {
			type: type,
			unit: unit
		}
	}

	// @param this - current rickshawFacade
	var extractAxesInformation = function(){

		// assert first series is representative in case of axes
		var firstSeries = this._seriesData[0];

		var xAttributeInformation = extractCategoryInformation(
			firstSeries.data.baseAccessor,
			firstSeries.xAttribute,
			firstSeries.data.categoryAccessorFunctorFactory,
			firstSeries.data.unitFunctor,
			firstSeries.data.typeFunctor
		);

		var yAttributeInformation = extractCategoryInformation(
			firstSeries.data.baseAccessor,
			firstSeries.yAttribute,
			firstSeries.data.categoryAccessorFunctorFactory,
			firstSeries.data.unitFunctor,
			firstSeries.data.typeFunctor
		);


		return {
			xAttributeInformation: xAttributeInformation,
			yAttributeInformation: yAttributeInformation
		}
	}

	// @param this - current rickshawFacade
	var setUpXAxis = function(info){

		// TODO: substitute type identifiers by 'constant' variables
		if(info.type == "number"){

			setUpXAxisNumber.call(this, info.unit);
		}
		else{
			if(info.type == "time"){

				setUpXAxisTime.call(this);
			}
			else{
				throw new Errow("type '"+type+"' is not supported for x axis");
			}
		}
	}

	// @param this - current RickshawFacade
	var setUpXAxisNumber = function(unit){

		this._xAxis = new Rickshaw.Graph.Axis.X(
			{
				graph: this._graph,
				tickFormat: function(date){
					return date.toString() + " " + info.unit;
				}
			}
		);
	}

	// @param this - current RickshawFacade
	var setUpXAxisTime = function(){

		this._xAxis = new Rickshaw.Graph.Axis.Time(
			{
				graph: this._graph
			}
		);
	}

	// @param this - current RickshawFacade
	var setUpYAxis = function(info){

		// TODO: substitute type identifiers by 'constant' variables
		if(info.type == "number"){

			setUpYAxisNumber.call(this, info.unit);
		}
		else{
			throw new Error("type '"+type+"' is not supported for y axis");
		}
	}

	// @param this - current RickshawFacade
	var setUpYAxisNumber = function(unit){

		this._yAxis = new Rickshaw.Graph.Axis.Y(
			{
				graph: this._graph,
				orientation: "left",
				element: this._yAxisDO,
				tickFormat: function(date){
					return date.toString() + " " + unit;
				}
			}
		);
	}

	// @param this - current rickshawFacade
	var setUpWidgets = function(){

		this._rangeSlider = new Rickshaw.Graph.RangeSlider( {
			graph: this._graph,
			element: this._rangeSliderDO
		} );

		this._legend = new Rickshaw.Graph.Legend({
			graph: this._graph,
			element: this._legendDO
		});

		var shelving = new Rickshaw.Graph.Behavior.Series.Toggle({
			graph: this._graph,
			legend: this._legend
		});
	}

	// @param this - current rickshawFacade
	var setUpGraph = function(){

		var graphSeries = generateGraphSeries.call(this);

		createGraph.call(this, graphSeries);

	}

	// @param this - current rickshawFacade
	var generateGraphSeries = function(){

		var colorPalette = new Rickshaw.Color.Palette();

		return this._seriesData.map(
			processSingleSeries,
			colorPalette
		);
	}

	/**
	 * @brief generates data series format for a rickshaw graph from a single given series
	 * @param this - a Rickshaw.Color.Palette
	 */
	var processSingleSeries = function(series){

		// sort to create series
		var compareFunctor = getCompareFunctor(
			series.sortingAttribute,
			series.data
		);
		var sortedAccesor = series.data.baseAccessor.sort(compareFunctor)

		// extract data
		var extractFunctor = getExtractFunctor(
			series.data,
			series.xAttribute,
			series.yAttribute
		);
		// apply extract functor to every element
		var data = sortedAccesor.split().map(extractFunctor);

		// assemble data and additional information
		return {
			name: series.name,
			data: data,
			color: this.color()
		};
	}

	/**
	 * @param data - {baseAccessor, categoryAccessorFunctorFactory, valueFunctor, ...}
	 */
	var getExtractFunctor = function(data, xAttribute, yAttribute){

		// assert first element is representative for others
		var exampleElement = data.baseAccessor.at(0);

		var xAttributeFunctor = data.categoryAccessorFunctorFactory(exampleElement, xAttribute);
		var yAttributeFunctor = data.categoryAccessorFunctorFactory(exampleElement, yAttribute);

		var xProcessDateFunctor = getProcessDateFunctor(xAttributeFunctor(exampleElement), data.typeFunctor);
		var yProcessDateFunctor = getProcessDateFunctor(yAttributeFunctor(exampleElement), data.typeFunctor);

		return function(elementAccessor){
			return {
				x: xProcessDateFunctor(xAttributeFunctor(elementAccessor).access()),
				y: yProcessDateFunctor(yAttributeFunctor(elementAccessor).access())
			};
		}
	}

	/**
	 * @brief returns a functor which can process a type of data to a type that can be processed by Richshaw
	 * this is esspecially important for processing timestamps as Rickshaw expects them in a certain number format
	 */
	var getProcessDateFunctor = function(categoryAccessor, typeFunctor){

		var type = typeFunctor(categoryAccessor).access();
		var processDateFunctor;

		// TODO: substitute literals with 'constants'
		if(type == "number"){
			processDateFunctor = RickshawFacade.standardNumberProcessDateFunctor;
		}
		else{
			if(type == "time"){
				processDateFunctor = RickshawFacade.standardTimeProcessDateFunctor;
			}
			else{
				throw new Error("type '"+type+"' is not supported for sorting");
			}
		}

		return processDateFunctor;
	}

	/**
	 * @param data - { baseAccessor, categoryAccessorFunctorFactory, typeFunctor, unitFunctor, valueFunctor }
	 */
	var getCompareFunctor = function(category, data){

		// assert first element is representative
		var exampleElement = data.baseAccessor.at(0);

		var categoryAccessorFunctor = data.categoryAccessorFunctorFactory(exampleElement, category);
		var categoryAccessor = categoryAccessorFunctor(exampleElement);

		var type = data.typeFunctor(categoryAccessor).access();

		var compareFunctor;

		// TODO: substitute type literals by 'constants'
		if(type == "number"){

			compareFunctor = RickshawFacade.standardNumberCompareFunctorFactory(
				categoryAccessor,
				data.valueFunctor
			);
		}
		else{
			if(type == "time"){

				compareFunctor = RickshawFacade.standardTimeCompareFunctorFactory(
					categoryAccessorFunctor,
					data.valueFunctor
				);
			}
			else{
				throw new Error("type '"+type+"' is not supported for sorting");
			}
		}

		return compareFunctor;
	}

	// @param this - current rickshawFacade
	var reset = function(){

		// reset DOM tree
		while(this._containerDO.childElementCount > 0){
			this._containerDO.removeChild(this._containerDO.firstChild);
		}

		setUpDOM.call(this);
	}

	// @param this - current
	var setUpDOM = function(){

		var getDOFrom = function(selection){

			return selection[0][0];
		}

		var containerSelection = d3.select(this._containerDO);

		var yAxisSelection = containerSelection.append("div")
			.attr("class", RickshawFacade.Y_AXIS_DO_CLASS);
		this._yAxisDO = getDOFrom(yAxisSelection);

		var group = containerSelection.append("div")
			.attr("class", RickshawFacade.CHART_AND_SLIDER_DO_CLASS);

		var chartSelection = group.append("div")
			.attr("class", RickshawFacade.CHART_DO_CLASS);
		this._chartDO = getDOFrom(chartSelection);

		var rangeSliderSelection = group.append("div")
			.attr("class", RickshawFacade.RANGESLIDER_DO_CLASS);
		this._rangeSliderDO = getDOFrom(rangeSliderSelection);

		var legendSelection = group.append("div")
			.attr("class", RickshawFacade.LEGEND_DO_CLASS);
		this._legendDO = getDOFrom(legendSelection);
	}

	// @param this - current rickshawFacade
	var createGraph = function(series){

		// standard set up
		this._graph = new Rickshaw.Graph(
			{
				element: this._chartDO,
				width: 700,
				height: 300,
				series: series,
				unstack: true
			}
		);

		// renderer
		this._graph.setRenderer(
			Rickshaw.Graph.Renderer.Line,
			{
				tension: 1,
				strokeWidth: 1,
				dotSize: 2,
				stroke: "true"
			}
		);
	}

	//############
	//### main ###
	//############

	this._seriesData = seriesData;

	reset.call(this);

	setUpGraph.call(this);
	setUpAxes.call(this);
	setUpWidgets.call(this);
}

RickshawFacade.prototype.render = function(){

	this._graph.render();
}


// TODO: move to definition of types (?)
RickshawFacade.standardTimeCompareFunctorFactory = function(categoryFunctor, valueFunctor){

	return function(elementAccessorA, elementAccessorB){

		return RickshawFacade.standardTimeCompareFunctor(
			valueFunctor(categoryFunctor(elementAccessorA)).access(),
			valueFunctor(categoryFunctor(elementAccessorB)).access()
		)
	}
}

RickshawFacade.standardTimeCompareFunctor = function(a, b){

	return a.getTime() - b.getTime();
}

RickshawFacade.standardNumberCompareFunctorFactory = function(categoryFunctor, valueFunctor){

	return function(elementAccessorA, elementAccessorB){

		return RickshawFacade.standardNumberCompareFunctor(
			valueFunctor(categoryFunctor(elementAccessorA)).access(),
			valueFunctor(categoryFunctor(elementAccessorB)).access()
		)
	}
}

RickshawFacade.standardNumberCompareFunctor = function(a, b){

	return a-b;
}

RickshawFacade.standardNumberProcessDateFunctor = function(date){

	return date;
}
/**
 * @see http://code.shutterstock.com/rickshaw/tutorial/introduction.html#example_03
 * @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getTime#Returns
 */
RickshawFacade.standardTimeProcessDateFunctor = function(date){

	return date.getTime()/1000;
}
