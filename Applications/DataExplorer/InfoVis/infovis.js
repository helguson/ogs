"use strict";

// define infovis package / conglomerate- / accumulation-object
window.infovis = {};

// removes error message of unavailability of JavaScript in case of availability of JavaScript
function removeMessageOfUnavailabilityOfJavaScript(){

	document.getElementById("javaScriptNotAvailableDummy").remove();
}

// TODO: think about the robustnes of this method
// TODO: test
// returns whether d3.js is available
function isD3Available(){

	return window["d3"] != undefined;
}

// @param accessor - list of rows (lists)
infovis.setUpTestDiagram = function(accessor){

	var width = accessor.length*infovis.testWidthFactor;
	var height = infovis.testHeight;

	var svgSelection = d3.select("#viz").append("svg");
	svgSelection
		.attr("xmlns", "http://www.w3.org/2000/svg")
		.attr("version", "1.1")
		.attr("width", width)
		.attr("height", height);

	// test axes
	//dim1 - time
	//TimestampMeasurement
	//dim2 - number
	//WeatherAirtemperature1mSingle_value15Min

	var dimXAccess = function(rowAccessor){
		return rowAccessor.access("TimestampMeasurement")[0];
	}
	var dimYAccess = function(rowAccessor){
		return rowAccessor.access("WeatherAirtemperature1mSingle_value15Min")[0];
	}

	// local extent
	// Question: may it be better to use some fixed borders?
	var dimXExtent = d3.extent(
		accessor,
		dimXAccess
	);
	var dimYExtent = d3.extent(
		accessor,
		dimYAccess
	);

	var xScale = d3.time.scale()
		.domain(dimXExtent)
		.range([0, width-1]);

	var yScale = d3.scale.linear()
		.domain(dimYExtent)
		.range([height-1, 0]);

	var circleSelection = svgSelection.selectAll("circle")
				.data(accessor);

	circleSelection.exit()
		.remove();

	var enteredCircles = circleSelection.enter()
		.append("circle")
	enteredCircles
		.attr(
			"r",
			2
		);

	circleSelection
		.attr(
			"cy",
			function(rowAccessor){
				return yScale(dimYAccess(rowAccessor));
			}
		)
		.attr(
			"cx",
			function(rowAccessor){
				return xScale(dimXAccess(rowAccessor));
			}
		);
};

//###########################
//### DataAccessorFactory ###
//###########################
infovis.DataAccessorFactory = function(values, metaDataRelation){

	var DataAccessor = infovis.DataAccessor;

	this.values = values;
	this.metaDataRelation = metaDataRelation;

	this.makeAccessorFrom = function(indexList){

		return infovis.DataAccessor.inject(indexList, this);
	};
}

//####################
//### DataAccessor ###
//####################
/**
 * a DataAccessor is an JavaScript array that stores indices (integers >= 0 or undefined) or other DataAccessors
 * DataAccessor is refered as 'accessor' in the following
**/
infovis.DataAccessor = {};
var DataAccessor = infovis.DataAccessor;

DataAccessor.isDataAccessor = function(object){
	var result = false;

	if(object.isDataAccessor !== undefined){
		result = object.isDataAccessor();
	}

	return result;
}

DataAccessor.inject = function(baseDataIndices, dataStorage){

	//#######################
	//### private methods ###
	//#######################
	/**
	* @brief applies 'injectPropertiesInto' to 'result'
	* and to every array that is stored within 'result'
	**/
	var injectProperties = function(base){

		var injectables = [base];

		while(injectables.length > 0){
			// dequeue first element
			var currentArray = injectables.shift();

			var additionalInjectables = currentArray.filter(Array.isArray);

			// enqueue additional elements
			injectables = injectables.concat(
				additionalInjectables
			);

			injectPropertiesInto(currentArray);
		}
	}

	var injectPropertiesInto = function(object){

		injectNonPrototypicPropertiesInto(object);
		injectPrototypicPropertiesInto(object, DataAccessor.prototype);
	};

	var injectNonPrototypicPropertiesInto = function(object){

		object.dataStorage = dataStorage;
	};

	var injectPrototypicPropertiesInto = function(object, prototype){

		for(var key in prototype){
			if(prototype.hasOwnProperty(key)){
				Object.defineProperty(
					object,
					key,
					{value: prototype[key]}
				);
			}
		}
	};

	//#############
	//### setUp ###
	//#############

	injectProperties(baseDataIndices);

	return baseDataIndices;
}

DataAccessor.prototype = {};

DataAccessor.prototype.access = function(value){
	//#######################
	//### private methods ###
	//#######################
	// @param this - current accessor
	var accessAll = function(){

		var thisAccessor = this;

		return this.map(
			function(element){
				// element is sub array
				if(Array.isArray(element)){
					return element.access();
				}
				// element is valueIndex
				else{
					return thisAccessor.dataStorage.values[element];
				}
			}
		)
	};

	// @param this - current accessor
	var accessElements = function(value){
		var result;

		if(typeof(value) == "function"){

			result = accessElementsByPredicateFunction.call(this, value);
		}
		else{

			result = accessElementsByMetaDate.call(this, value);
		}

		return result;
	};

	// @param this - current accessor
	var accessElementsByMetaDate = function(metaDate){

		return accessElementsByPredicateFunction.call(
			this,
			function(index, element){

				var metaData = element.meta()[0].access();

				// "return metaData.contains(metaDate);"
				return metaData.indexOf(metaDate) != -1;
			}
		);
	};

	/**
	 * @brief returns list of values which return true on application of predicate function
	 * @param this - current accessor
	 * @param predicateFunction(index, element)
	 * 	this - current accessor
	 * 	index - index of element within the current accessor
	 * 	element - accessor for the current element
	**/
	var accessElementsByPredicateFunction = function(predicateFunction){

		var result = [];

		for(var internalIndex = 0; internalIndex < this.length; internalIndex++){

			var internalElement = this[internalIndex];

			if(Array.isArray(internalElement)){

				var values = internalElement.access(predicateFunction);

				if(values.length > 0){
					result.push(values);
				}
			}
			else{

				var elementAccessor = DataAccessor.inject(
					[this[internalIndex]],
					this.dataStorage
				);

				var fulfiledPredicate = predicateFunction.call(
					this,
					internalIndex,
					elementAccessor
				);

				if(fulfiledPredicate){

					var value = elementAccessor.access()[0];
					result.push(value);
				}
			}

		}

		return result;
	};



	//############
	//### main ###
	//############
	var result;

	if(arguments.length == 0){
		result = accessAll.call(this);
	}
	else{
		result = accessElements.call(this, value);
	}

	return result;
};

DataAccessor.prototype.meta = function(value){
	//#######################
	//### private methods ###
	//#######################
	// @param this - current accessor
	var metaLists = function(){

		var thisAccessor = this;

		var listOfMetaAccessors = thisAccessor.map(
			function(element){

				if(Array.isArray(element)){

					return element.meta();
				}
				else{
					// create copy instead of modifying stored data
					var metaDataIndices = [].concat(
						thisAccessor.dataStorage.metaDataRelation[element]
					);

					return DataAccessor.inject(
						metaDataIndices,
						thisAccessor.dataStorage
					);
				}
			}
		);

		return DataAccessor.inject(
			listOfMetaAccessors,
			thisAccessor.dataStorage
		);
	};

	// @param this - current accessor
	var metaListBy = function(value){
		var result;

		if(typeof(value) == "function"){

			result = metaListByPredicateFunction.call(this, value);
		}
		else{

			result = metaListByMetaDate.call(this, value);
		}

		return result;
	};


	/**
	 * @param this - current accessor
	 * @param predicateFunction
	 * 	this - current accessor
	 * 	index - index within list of meta data for a date
	 * 	metaElement - accessor for current meta date
	**/
	var metaListByPredicateFunction = function(predicateFunction){

		var thisAccessor = this;

		var resultIndices = thisAccessor.map(
			function(element){

				if(Array.isArray(element)){

					return element.meta(predicateFunction);
				}
				else{

					var result = undefined;

					var metaDataIndices = thisAccessor.dataStorage.metaDataRelation[element];

					for(var index = 0; index < metaDataIndices.length; index++){

						var metaDate = DataAccessor.inject(
							[metaDataIndices[index]],
							thisAccessor.dataStorage
						);

						var fulfiledPredicate = predicateFunction.call(
							thisAccessor,
							index,
							metaDate
						);

						if(fulfiledPredicate){

							result = metaDataIndices[index];
							break;
						}
					}

					return result;
				}
			}
		);

		return DataAccessor.inject(resultIndices, thisAccessor.dataStorage);
	};

	// @param this - current accessor
	var metaListByMetaDate = function(metaMetaDate){

		return metaListByPredicateFunction.call(
			this,
			function(index, metaElement){

				var metaMetaData = metaElement.meta()[0].access();

				// "return metaMetaData.contains(metaMetaDate);"
				return metaMetaData.indexOf(metaMetaDate) != -1;
			}
		);
	};

	//############
	//### main ###
	//############
	var result;

	if(arguments.length == 0){
		result = metaLists.call(this);
	}
	else{
		result = metaListBy.call(this, value);
	}

	return result;
}

DataAccessor.prototype.isDataAccessor = function(){

	return true;
}

//###################
//### inheritance ###
//###################
/**
 * mechanism to emulate singular inheritance
 * This mechanism chains prototypes in order to create a child-parent-relation.
 * This mechanism consist of two methods, one for chaining prototypes to declare
 * parents (extend) and one for calling the parent constructor within a
 * constructor (applyParentConstructorTo). These methods are extensions of
 * Function.prototype. By this they are members of the constructor function.
 *
 * example:
 * <code>
 * Parent = function(){
 * 	this.adultStuff = 300;
 * };
 *
 * Parent.prototype.getAdultStuff = function(){
 * 	return "This is Sparta!";
 * }
 *
 * Child = function(){
 * 	var resultInstance = Child.applyParentConstructorTo(this);
 *
 * 	resultInstance.childStuff = 1*1;
 * }
 * Child.extend(Parent);
 *
 * Child.prototype.getChildishStuff = function(){
 * 	return this.childStuff;
 * }
 *
 * var childInstance = new Child();
 * </code>
 */

/**
 * @method Function.prototype.applyParentConstructorTo(instance, args
 * [, constructorReplacedInstanceHandler])
 *
 * @brief if parent constructor exists, applies it on given instance with given arguments
 * @throws exception if constructor replaced initial instance and no handler is given
 *
 * @param this - current class (constructor)
 *
 * @param instance - current instance on which the parent constructor shall be applied
 *
 * @param arguments - arguments for parent constructor
 *
 * @param constructorReplacedInstanceHandler(initialInstance, constructorResultInstance) -
 * function that is called if constructor replaced initial instance by another instance.
 * (i. e. Array)
 * 	@param this - current class (constructor)
 *
 * 	@param initialInstance - instance that was initially used to call constructor
 *
 * 	@param constructorResultInstance - instance that was returned by constructor
 *
 * @returns result of constructor call
 */
Function.prototype.applyParentConstructorTo = function(instance, args, constructorReplacedInstanceHandler){
	//#######################
	//### private methods ###
	//#######################
	var handleReplacedInstance = function(initialInstance, constructorResultInstance, handler){

		if(typeof constructorReplacedInstanceHandler !== "function"){

			throw new Error("no handler is given for case: constructor replaced initial instance");
		}
		else{

			return handler.call(this, initialInstance, constructorResultInstance);
		}
	};

	//############
	//### main ###
	//############
	var parentPrototype = Object.getPrototypeOf(this.prototype);
	var resultInstance = instance;

	if(parentPrototype !== null){

		var parentConstructor = parentPrototype.constructor;

		resultInstance = parentConstructor.apply(instance, args);

		var constructorReplacedInstance = instance !== resultInstance;
		if(constructorReplacedInstance){

			resultInstance = handleReplacedInstance.call(
				this,
				instance,
				resultInstance,
				constructorReplacedInstanceHandler
			);
		}
	}

	return resultInstance;
}

if (typeof Object.setPrototypeOf !== "function"){
	Object.setPrototypeOf = function(object, goalPrototype){
		object.__proto__ = goalPrototype;
	}
}

/**
 * @brief emulates inheritance by chaining prototypes
 * @param this - child class (constructor)
 * @param parentClass - parent class (constructor)
 * notice, this function requires to define the constructor first and the parent
 * relation secondly
 */
Function.prototype.extend = function(parentClass){

	Object.setPrototypeOf(this.prototype, parentClass.prototype);
}


//#################
//### SVGCanvas ###
//#################
// constructor
infovis.Canvas = function(svgParentIdentifier){

	//#######################
	//### private methods ###
	//#######################
	var setUpSVG  = function(){

		this.svgSelection = d3.select(svgParentIdentifier).append("svg");

		this.svgSelection
			.attr("xmlns", "http://www.w3.org/2000/svg")
			.attr("version", "1.1")
	};

	var setUpListener = function(){

		var thisDOMElement = this.svgSelection[0][0];

		this.onWindowResizeListener = function(event){

			thisDOMElement.dispatchEvent(new Event("resize"));
		};
	}

	var setUpEvents = function(){

		var thisCanvas = this;

		this.svgSelection.on(
			"resize",
			function(date, index){
			// @param this - current DOM element
				var computedStyle = window.getComputedStyle(this, null);
				var width = window.parseInt(computedStyle.width);
				var height = window.parseInt(computedStyle.height);

				thisCanvas.onResize(width, height);
			}
		);

		window.addEventListener("resize", thisCanvas.onWindowResizeListener);
	};


	//############
	//### main ###
	//############
	setUpSVG.call(this);
	setUpListener.call(this);
	setUpEvents.call(this);
}

infovis.Canvas.prototype.svgSelection = undefined;

infovis.Canvas.prototype.onWindowResizeListener = undefined;

infovis.Canvas.prototype.onResize = function(width, height){

	console.log([width, height]);
}

infovis.Canvas.prototype.beforeRemove = function(){

	window.removeEventListener("resize", this.onWindowResizeListener);
}

//####################
//### line diagram ###
//####################

infovis.LineDiagram = function(svgParentIdentifier, dataAccessor){

	//############
	//### main ###
	//############
	var result = infovis.LineDiagram.applyParentConstructorTo(this, [svgParentIdentifier]);

	result.setUpDiagram(dataAccessor);

	return result;
}
infovis.LineDiagram.extend(infovis.Canvas);
