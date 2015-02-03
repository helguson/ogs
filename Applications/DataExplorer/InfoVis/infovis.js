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

infovis.testWidthFactor = 2;
infovis.testHeight = 300;

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

		return rowAccessor.findFirst(
				function(elementAccessor){
					return elementAccessor.meta("name").access() == "TimestampMeasurement";
				}
			)
			.access();
	}
	var dimYAccess = function(rowAccessor){
		return rowAccessor.findFirst(
				function(elementAccessor){
					return elementAccessor.meta("name").access() == "WeatherAirtemperature1mSingle_value15Min";
				}
			)
			.access();
	}

	// local extent
	// Question: may it be better to use some fixed borders?
	var dimXExtent = d3.extent(
		accessor.split(),
		dimXAccess
	);
	var dimYExtent = d3.extent(
		accessor.split(),
		dimYAccess
	);

	var xScale = d3.time.scale()
		.domain(dimXExtent)
		.range([0, width-1]);

	var yScale = d3.scale.linear()
		.domain(dimYExtent)
		.range([height-1, 0]);

	var circleSelection = svgSelection
		.datum(accessor)
		.selectAll("circle")
			.data(
				function(svgDatum){
					return svgDatum.split();
				}
			);

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

//### QtWebkit fixes ###
if (typeof Object.setPrototypeOf !== "function"){
	Object.setPrototypeOf = function(object, goalPrototype){
		object.__proto__ = goalPrototype;
	}
}

//### methods ###
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

	/// @see extend
	var getParentConstructor = function(){

		var parentConstructor;

		if(this.hasOwnProperty("parentConstructor")){

			parentConstructor = this.parentConstructor;
		}
		else{

			parentConstructor = parentPrototype.constructor;
		}

		return parentConstructor;
	}

	//############
	//### main ###
	//############
	var parentPrototype = Object.getPrototypeOf(this.prototype);
	var resultInstance;

	if(parentPrototype != undefined){

		var parentConstructor = getParentConstructor.apply(this);

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
	else{
		resultInstance = instance;
	}

	return resultInstance;
}

/**
 * extend(parentClass[, parentConstructor])
 * @brief emulates inheritance by chaining prototypes
 * @param this - child class (constructor)
 * @param parentClass - parent class (constructor)
 * @param parentConstructor - constructor function if you need to use a
 * different constructor than the standard constructor (prototype.constructor),
 * will assigned to the inheriting class as attribute 'parentConstructor'
 * notice, this function requires to define the constructor first and the parent
 * relation secondly
 */
Function.prototype.extend = function(parentClass, parentConstructor){

	Object.setPrototypeOf(this.prototype, parentClass.prototype);

	if(arguments.length >= 2){
		this.parentConstructor = parentConstructor;
	}
}

//########################
//### inheritance test ###
//########################
var accessTest = function(arrayOfInstances, numberOfReplications, numberOfRepetitions, executionHandler){
	//#######################
	//### private methods ###
	//#######################
	/**
	 * @brief measures elapsed time for sequentially repeated excution of a given method on a given object
	 * @param instance - object that will be given to executionHandler as parameter
	 * @param numberOfRepetitions - number of executionHandler calls
	 * @param executionHandler(instance) - function whose execution duration will be measured
	 * 	@param instance - instance given to testInstance
	 * @returns elapsed time in ms
	 */
	var testInstance = function(instance, numberOfRepetitions, executionHandler){

		var startTime = Date.now();

		for(var repetitionIndex = 0; repetitionIndex < numberOfRepetitions; repetitionIndex++){

			executionHandler.call(undefined, instance);
		}

		var endTime = Date.now();

		var elapsedTime = endTime - startTime;

		return elapsedTime;
	};

	//############
	//### main ###
	//############
	var times = [];

	for(var replicationIndex = 0; replicationIndex < numberOfReplications; replicationIndex++){

		var currentTime = arrayOfInstances.map(
			function(instance, indexOfInstance, originalArray){

				return testInstance(instance, numberOfRepetitions, executionHandler);
			}
		);

		times.push(currentTime);
	}

	return times;
}

var class1 = function(){

	this.i = 0;
	this.do = function(){
		this.i++;
	}
}

var class2 = function(){ return this; }
class2.prototype.i = 0;
class2.prototype.do = function(){
	this.i++;
}

var class3 = function(){

	return window.class3.applyParentConstructorTo(this);
}
class3.extend(class2);

var getInstances = function(){
	return [
		new class1(),
		new class2(),
		new class3()
	];
}

var executionHandler = function(instance){
	instance.do();
}

var analyseTestResults = function(times){

}

var analyse01 = function(times){

	var numberOfReplications = times.length;
	var numberOfInstances = times[0].length;

	var result = new Array(numberOfInstances);
	result = result.fill(0);

	times.forEach(
		function(replicationResults, replicationIndex){

			replicationResults.forEach(
				function(elapsedTime, index){

					result[index] += elapsedTime;
				}
			);
		}
	);

	result = result.map(
		function(sum, index){
			return sum / numberOfReplications
		}
	);

	return result;
}

var analyse02 = function(times){

	var numberOfReplications = times.length;
	var numberOfInstances = times[0].length;

	var result = new Array(numberOfInstances);
	result = result.fill(0);

	times.forEach(
		function(replicationResults, replicationIndex){

			var resultWeight = replicationIndex;
			var nextElementWeight = 1;
			var totalWeight = resultWeight + nextElementWeight;

			replicationResults.forEach(
				function(elapsedTime, index){

					var currentResult = result[index];
					var nextElement = elapsedTime;

					result[index] = currentResult*(resultWeight/totalWeight)+nextElement*(nextElementWeight/totalWeight);
				}
			);
		}
	);

	return result;
}


//###########################
//### DataAccessorFactory ###
//###########################
infovis.DataAccessorFactory = function(values, metaDataRelation){

	var DataAccessor = infovis.DataAccessor;

	this.values = values;
	this.metaDataRelation = metaDataRelation;

	this.makeAccessorFrom = function(indexList){

		return new infovis.DataAccessor(indexList, this);
	};
}

//###############
//### Wrapper ###
//###############
/**
 * @brief a wrapper class especially for wrapping primitive types
 */
infovis.Wrapper = function(element){
	this.element = element;

	return this;
}


//#####################
//### DatumAccessor ###
//#####################

//### constructor ###
infovis.DatumAccessor = function(datumIndex, dataStorage){

	var resultInstance = infovis.DatumAccessor.applyParentConstructorTo(
		this,
		[datumIndex]
	);

	resultInstance.dataStorage = dataStorage;

	return resultInstance;
}
infovis.DatumAccessor.extend(infovis.Wrapper);

var DatumAccessor = infovis.DatumAccessor;

DatumAccessor.prototype.access = function(){

	return this.dataStorage.values[this.element];
}

DatumAccessor.prototype.meta = function(value){
	//#######################
	//### private methods ###
	//#######################

	/**
	 * @param this - current accessor
	 */
	var getMetaData = function(){

		return new infovis.DataAccessor(
			this.dataStorage.metaDataRelation[this.element],
			this.dataStorage
		);
	}

	/**
	 * @param this - current accessor
	 */
	var getMetaDatum = function(value){

		if(typeof value == "function"){
			return getMetaDatumByPredicateFunctor.call(
				this,
				value
			)
		}
		else{
			return getMetaDatumByMetaMetaValue.call(
				this,
				value
			)
		}
	}

	/**
	 * @brief returns accessor to the first metaDatum which fulfils the
	 * given predicate
	 * @param this - current accessor
	 * @param predicateFunctor([metaDatumAccessor[, descendingIndex[, originalDataAccessor]]])
	 * 	@param metaDatumAccessor - DatumAccessor to a metaDatum of the current accessor
	 * 	@param descendingIndex - regarding the metaData list of the current accessor
	 * 	@param metaDataAccessor - accessor for the metaData list of the current accessor
	 */
	var getMetaDatumByPredicateFunctor = function(predicateFunctor){

		return this.meta().findFirst(predicateFunctor);
	}

	/**
	 * @brief returns accessor to the first metaDatum which has a given
	 * metaMetaDatum (in relation to the current datum)
	 * @example get name for a given datum: datumAccessor.meta("name")
	 * @param this - current accessor
	 */
	var getMetaDatumByMetaMetaValue = function(metaMetaValue){

		return getMetaDatumByPredicateFunctor.call(
			this,
			function(metaDatumAccessor, descendingIndex, originalDataAccessor){

				var metaMetaValues = metaDatumAccessor.meta().access();

				return metaMetaValues.indexOf(metaMetaValue) != -1;
			}
		)
	}

	//############
	//### main ###
	//############
	var result;

	if(arguments.length == 0){
		result = getMetaData.call(this);
	}
	else{
		result = getMetaDatum.call(this, value);
	}

	return result;
}

DatumAccessor.prototype.innerData = function(){
	return this.element;
}

//####################
//### DataAccessor ###
//####################
/**
 * a DataAccessor is an JavaScript array that stores indices (integers >= 0 or undefined) or other DataAccessors
 * DataAccessor is refered as 'accessor' in the following
**/

//### QtWebkit fixes ###
if (typeof Array.of !== "function"){
	Array.of = function(){
		var numberOfElements = arguments.length;
		var result = new Array(numberOfElements);

		for(var i = 0; i < numberOfElements; i++){
			result[i] = arguments[i];
		}

		return result;
	}
}

//### constructor ###
infovis.DataAccessor = function(dataIndices, dataStorage){
	//#######################
	//### private methods ###
	//#######################

	// as this is what arrays do in Firefox
	var parentConstructorReplacedInitialInstanceHandler = function(initialInstance, constructorResultInstance){

		Object.setPrototypeOf(
			constructorResultInstance,
			Object.getPrototypeOf(initialInstance)
		);

		return constructorResultInstance;
	}

	//############
	//### main ###
	//############
	if(!Array.isArray(dataIndices)){
		throw new Error("dataIndices must be an array");
	}
	else{

		var resultInstance = infovis.DataAccessor.applyParentConstructorTo(
			this,
			dataIndices,
			parentConstructorReplacedInitialInstanceHandler
		);

		resultInstance.dataStorage = dataStorage;

		return resultInstance;
	}
}
// as Array constructor returns with a single number argument N rather an array
// with N elements that an array with the element N
infovis.DataAccessor.extend(Array, Array.of);

var DataAccessor = infovis.DataAccessor;

DataAccessor.isDataAccessor = function(object){
	var result = false;

	if(object.isDataAccessor !== undefined){
		result = object.isDataAccessor();
	}

	return result;
}

/**
 * @asserts index is Number or index is Array of Numbers
 */
infovis.DataAccessor.prototype.at = function(index){

	//#######################
	//### private methods ###
	//#######################
	var getInnerElementAt = function(index){

		var innerElement = undefined;

		if(Array.isArray(index)){
			var descendingIndex = index;

			innerElement = this;

			Array.prototype.forEach.call(
				descendingIndex,
				function(index){
					innerElement = innerElement[index];
				}
			);
		}
		else{
			innerElement = this[index];
		}

		return innerElement;
	}

	//############
	//### main ###
	//############
	var result = undefined;
	var innerElement = getInnerElementAt.call(
		this,
		index
	);


	if(Array.isArray(innerElement)){
		result = new infovis.DataAccessor(
			innerElement,
			this.dataStorage
		);
	}
	else{
		result = new infovis.DatumAccessor(
			innerElement,
			this.dataStorage
		);
	}

	return result;
}

/**
 * @brief applies functor for every contained element
 * @param functor([element[, descendingIndex[, originalDataAccessor]]])
 * 	@param this - given context or undefined
 * 	@param element - DatumAccessor (or DataAccessor if
 * 	applyFunctorToArrayElements == true) for current element
 * 	@param descendingIndex - of current element
 * 	@param originalDataAccessor -
 * @param context - object to use as this for functor
 * @param applyFunctorToArrayElements -
 */
DataAccessor.prototype.forEach = function(functor, context, applyFunctorToArrayElements){
	//########################
	//### helper variables ###
	//########################
	var appliableDescendingIndices;

	//#######################
	//### private methods ###
	//#######################
	var setUp = function(){
		appliableDescendingIndices = [];

		var rootDescendingIndex = [];
		appliableDescendingIndices.push(rootDescendingIndex);

		context = (arguments.length >= 2)?
			context
			: undefined;

		applyFunctorToArrayElements = (arguments.length >= 3)?
			applyFunctorToArrayElements
			: false;
	}

	var hasNextDescendingIndex = function(){

		return appliableDescendingIndices.length > 0;
	}

	var getNextDescendingIndex = function(){

		return appliableDescendingIndices.shift();
	}

	/**
	 * @param this - current DataAccessor
	 */
	var handleArray = function(arraysDescendingIndex, array){

		var childrensDescendingIndices = Array.prototype.map.call(
			array,
			function(element, index){

				return determineDescendingIndexFrom(
					arraysDescendingIndex,
					index
				);
			}
		);

		prependDescendingIndices(childrensDescendingIndices);
	}

	var determineDescendingIndexFrom = function(parentDescendingIndex, indexInParent){

		return parentDescendingIndex.concat(indexInParent);
	}

	var prependDescendingIndices = function(descendingIndices){

		appliableDescendingIndices = descendingIndices.concat(appliableDescendingIndices);
	}

	//############
	//### main ###
	//############

	setUp();

	while(hasNextDescendingIndex()){

		var currentDescendingIndex = getNextDescendingIndex();
		var currentAccessor = this.at(currentDescendingIndex);

		var elementIsArray = infovis.DataAccessor.isDataAccessor(currentAccessor);
		if(elementIsArray){

			handleArray.call(
				this,
				currentDescendingIndex,
				currentAccessor
			);
		}

		//       | applyFunctorToArrayElements
		//       |       0             1
		// -----------------------------------
		// array |
		//   0   |       1             1
		//   1   |       0             1
		if(
			!elementIsArray
			|| applyFunctorToArrayElements
		){

			functor.call(
				context,
				currentAccessor,
				currentDescendingIndex,
				this
			);
		}

	}
}

/**
 * @param mappingFunctor([element[, descendingIndex[, originalDataAccessor]]])
 * 	@param this - given context or undefined
 * 	@param element - DatumAccessor for current element
 * 	@param descendingIndex - of current element
 * 	@param originalDataAccessor -
 * @param context - object to use as this for mappingFunctor
 */
DataAccessor.prototype.map = function(mappingFunctor, context){

	//###########################################
	//### private closure-independant methods ###
	//###########################################
	/**
	 * @param array const -
	 * @asserts non cyclic structure
	 */
	var createStructuralCopyOf = function(array){

		var resultStructure = new Array(array.length);

		var unsearchedDescendingIndices = [];
		unsearchedDescendingIndices.push([]); // root

		while(unsearchedDescendingIndices.length > 0){
			var currentDescendingIndex = unsearchedDescendingIndices.shift();
			var currentArray = getElement(currentDescendingIndex, array);

			Array.prototype.forEach.call(
				currentArray,
				function(element, index){

					if(Array.isArray(element)){

						var newArrayDescendingIndex = currentDescendingIndex.concat(index);
						setElement(
							newArrayDescendingIndex,
							resultStructure,
							new Array(element.length)
						);
						unsearchedDescendingIndices.push(newArrayDescendingIndex);
					}
				}
			);
		}

		return resultStructure;
	}

	/**
	 * @param descendingIndex const -
	 * @param root const -
	 */
	var getElement = function(descendingIndex, root){

		var element = root;

		descendingIndex.forEach(
			function(indexInParent){
				element = element[indexInParent];
			}
		);

		return element;
	}

	/**
	 * @brief sets an element - that is not the root - in the the structure
	 * that is referenced by the root
	 * @asserts array structure
	 * @param descendingIndex const -
	 * @param root -
	 * @param element const -
	 */
	var setElement = function(descendingIndex, root, element){

		var indexInParent = descendingIndex[descendingIndex.length-1];
		var parentDescendingIndex = descendingIndex.slice(0, -1);

		var parent = getElement(parentDescendingIndex, root);

		parent[indexInParent] = element;
	}

	//########################
	//### helper variables ###
	//########################
	var result;

	//#######################
	//### private methods ###
	//#######################
	/// @param this - current DataAccessor
	var setUp = function(){

		result = createStructuralCopyOf(this);

		context = (arguments.length >= 2)?
			context
			: undefined;
	}

	var functor = function(accessor, descendingIndex, originalDataAccessor){

		var mappedElement = mappingFunctor.call(
			context,
			accessor,
			descendingIndex,
			originalDataAccessor
		);

		setElement(
			descendingIndex,
			result,
			mappedElement
		);
	}

	var getResult = function(){
		return result;
	}

	//############
	//### main ###
	//############

	setUp.call(this);

	infovis.DataAccessor.prototype.forEach.call(
		this,
		functor
	);

	return getResult();
}

DataAccessor.prototype.access = function(value){

	return infovis.DataAccessor.prototype.map.call(
		this,
		function(datumAccessor, descendingIndex, originalDataAccessor){

			return datumAccessor.access();
		}
	);
};


DataAccessor.prototype.meta = function(){

	var args = arguments;

	var metaDataIndices = this.map(
		function(elementAccessor, elementDescendingIndex, originalAccessor){

			return elementAccessor.meta.apply(
				elementAccessor,
				args
			).innerData();
		}
	);

	return new DataAccessor(
		metaDataIndices,
		this.dataStorage
	)
}

DataAccessor.prototype.isDataAccessor = function(){

	return true;
}

/**
 * @method split() const
 * @brief moves accessor layer one step downwards
 * @return array of accessors that are one step lower in the hierachy
 */
DataAccessor.prototype.split = function(){

	var thisAccessor = this;

	return Array.prototype.map.call(
		thisAccessor,
		function(innerElement, index){

			return thisAccessor.at(index);
		}
	);
}

/**
 * @param predicateFunctor([element[, descendingIndex[, originalDataAccessor]]])
 * 	@param this - given context or undefined
 * 	@param element - DatumAccessor for current element
 * 	@param descendingIndex - of current element
 * 	@param originalDataAccessor -
 * @param context - object to use as this for predicateFunctor
 */
// TODO: implement break on "predicate returns true"
DataAccessor.prototype.findFirst = function(predicateFunctor, context){

	var foundElementThatFulfilledPredicate = false;
	var result = undefined;
	context = (arguments.length>=2)? context: undefined;

	this.forEach(
		function(elementAccessor, descendingIndex, originalDataAccessor){

			if(!foundElementThatFulfilledPredicate){

				var predicateResult = predicateFunctor.call(
					context,
					elementAccessor,
					descendingIndex,
					originalDataAccessor
				);

				if(predicateResult == true){

					foundElementThatFulfilledPredicate = true;
					result = elementAccessor;
				}
			}
		}
	)

	return result;
}

DataAccessor.prototype.innerData = function(){
	return [].concat(this);
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

	return this;
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

	//result.setUpDiagram(dataAccessor);

	return result;
}
infovis.LineDiagram.extend(infovis.Canvas);
