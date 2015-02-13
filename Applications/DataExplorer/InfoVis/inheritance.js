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
