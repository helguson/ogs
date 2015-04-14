"use strict";

(function(container){

//##############################
//### CallSequencerInterface ###
//##############################
container.CallSequencerInterface = function(){

	return this;
}

/**
 * @brief adds a given call whose execution is finished when it returns
 * @param call - function()
 * 	@brief function is called without parameters, return value is ignored
 * 	@param this - undefined
 */
container.CallSequencerInterface.prototype.addSynchronousCall = function(call){
	throw new Error("this method needs to be implemented in a derived class");
}


/**
 * @brief adds a given call that informs by itsself the callSequencer at the end
 * of its execution
 * @param call - function(endOfExecutionNotifier)
 * 	@brief return value is ignored
 * 	@param this - undefined
 * 	@param endOfExecutionNotifier
 * 		@brief object that can be used one time only to notify the
 * 		CallSequencer that call has reached its end of execution
 * 		@method notify
 * 			@brief - neither parameters, nor return value
 */
container.CallSequencerInterface.prototype.addAsynchronousCall = function(call){
	throw new Error("this method needs to be implemented in a derived class");
}

//#############################
//### AbstractCallSequencer ###
//#############################
container.AbstractCallSequencer = function(){

	var instance = container.AbstractCallSequencer.applyParentConstructorTo(
		this,
		[]
	);

	instance._isTriggering = false;

	return instance;
}
container.AbstractCallSequencer.extend(container.CallSequencerInterface);

/**
 * @brief implementation of CallSequencerInterface.addAsynchronousCall
 * @see CallSequencerInterface.prototype.addAsynchronousCall
 */
container.AbstractCallSequencer.prototype.addAsynchronousCall = function(call){

	this._handleNewCall(call);

	this._trigger();
}

/**
 * @brief implementation of CallSequencerInterface.addSynchronousCall
 * @see CallSequencerInterface.prototype.addSynchronousCall
 */
container.AbstractCallSequencer.prototype.addSynchronousCall = function(call){

	var wrappedCall = function(endOfExecutionNotifier){

		call.call(undefined);

		endOfExecutionNotifier.notify();
	}

	this.addAsynchronousCall(wrappedCall);
}

/**
 *
 */
container.AbstractCallSequencer.prototype._trigger = function(){
	if(!this._isTriggering){
		this._isTriggering = true;

		//#######################
		//### private methods ###
		//#######################
		var wrap = function(call, endOfExecutionNotifier){

			return function(){
				call.call(undefined, endOfExecutionNotifier);
			};
		}

		//############
		//### main ###
		//############

		while(this._canCall() && this._shouldCall()){

			var endOfExecutionNotifier = new container.AbstractCallSequencer._EndOfExecutionNotifier(this);
			var call = this._getNextCall();
			var wrappedCall = wrap(call, endOfExecutionNotifier);

			this._issueCall(wrappedCall);
		}

		this._isTriggering = false;
	}
}

container.AbstractCallSequencer.prototype._handleEndOfExecution = function(){

	this._processEndOfExecution();

	this._trigger();
}


//### abstract methods ###
/**
 *
 */
container.AbstractCallSequencer.prototype._handleNewCall = function(call){
	throw new Error("this method needs to be implemented in a derived class");
}

/**
 * @ returns if an additional call can be issued
 */
container.AbstractCallSequencer.prototype._canCall = function(){
	throw new Error("this method needs to be implemented in a derived class");
}

/**
 * @ returns if an additional call should be issued asserting it is possible
 */
container.AbstractCallSequencer.prototype._shouldCall = function(){
	throw new Error("this method needs to be implemented in a derived class");
}

/**
 *
 */
container.AbstractCallSequencer.prototype._processEndOfExecution = function(){
	throw new Error("this method needs to be implemented in a derived class");
}

/**
 *
 */
container.AbstractCallSequencer.prototype._getNextCall = function(){
	throw new Error("this method needs to be implemented in a derived class");
}

/**
 * @brief starts execution of given call
 * depending on the implementation of this method the sequential execution happens
 * synchronous or asynchronous
 * @param call - function with bound endOfExecutionNotifier, so neither parameters
 * nor return value
 */
container.AbstractCallSequencer.prototype._issueCall = function(call){
	throw new Error("this method needs to be implemented in a derived class");
}

//### nested class ###
container.AbstractCallSequencer._EndOfExecutionNotifier = function(callSequencer){

	var notified = false;

	this.notify = function(){

		if(!notified){
			notified = true;

			callSequencer._handleEndOfExecution();
		}
	}

	return this;
}

//###########################
//### SimpleCallSequencer ###
//###########################
container.SimpleCallSequencer = function(){

	var instance = container.SimpleCallSequencer.applyParentConstructorTo(
		this,
		[]
	);

	instance._numberOfCurrentlyIssuedCalls = 0;
	instance._callList = [];

	return instance;
}
container.SimpleCallSequencer.extend(container.AbstractCallSequencer);

/**
 * @brief implementation of AbstractCallSequencer._handleNewCall
 * @see AbstractCallSequencer.prototype._handleNewCall
 */
container.SimpleCallSequencer.prototype._handleNewCall = function(call){

	this._callList.push(call);
}

/**
 * @brief implementation of AbstractCallSequencer._canCall
 * @see AbstractCallSequencer.prototype._canCall
 */
container.SimpleCallSequencer.prototype._canCall = function(){

	return this._callList.length > 0;
}

/**
 * @brief implementation of AbstractCallSequencer._shouldCall
 * @see AbstractCallSequencer.prototype._shouldCall
 */
container.SimpleCallSequencer.prototype._shouldCall = function(){

	return this._numberOfCurrentlyIssuedCalls < 1;
}

/**
 * @brief implementation of AbstractCallSequencer._processEndOfExecution
 * @see AbstractCallSequencer.prototype._processEndOfExecution
 */
container.SimpleCallSequencer.prototype._processEndOfExecution = function(){

	this._numberOfCurrentlyIssuedCalls -= 1;
}

/**
 * @brief implementation of AbstractCallSequencer._getNextCall
 * @see AbstractCallSequencer.prototype._getNextCall
 */
container.SimpleCallSequencer.prototype._getNextCall = function(){

	// remove first element E from list L and return E
	return this._callList.shift();
}

/**
 * @brief asynchronous implementation of AbstractCallSequencer._issueCall
 * @see AbstractCallSequencer.prototype._issueCall
 */
container.SimpleCallSequencer.prototype._issueCall = function(call){

	this._numberOfCurrentlyIssuedCalls += 1;

	setTimeout(
		call,
		0 // start (almost) immediate execution
	)
}

//#############################
//### DroppingCallSequencer ###
//#############################
container.DroppingCallSequencer = function(callCapacity){

	var instance = container.DroppingCallSequencer.applyParentConstructorTo(
		this,
		[]
	);

	instance._callCapacity = callCapacity;

	return instance;
}
container.DroppingCallSequencer.extend(container.SimpleCallSequencer);

/**
 * @brief implementation of AbstractCallSequencer._handleNewCall
 * @see AbstractCallSequencer.prototype._handleNewCall
 */
container.DroppingCallSequencer.prototype._handleNewCall = function(call){

	if(this._callList.length == this._callCapacity){
		// drop call
		this._callList.shift();
	}

	// call parent _handleNew
	// TODO: create abbreviation in inheritance.js
	container.DroppingCallSequencer.prototype.__proto__.constructor.prototype._handleNewCall.call(this, call);
}

})(window);
