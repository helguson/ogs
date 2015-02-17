"use strict";

window.historyPattern = {};


//###############
//### History ###
//###############
historyPattern.History = function(){

	//### helper variables ###
	var self = this;

	//##########################
	//### private attributes ###
	//##########################
	var undoList = [];
	var redoList = [];

	//#######################
	//### publish methods ###
	//#######################
	this.addAndDo = function(command){

		command.do();

		undoList.push(command);
		redoList = [];
	}

	this.undo = function(){

		if(self.canUndo()){
			var command = undoList.pop();

			command.undo();

			redoList.push(command);
		}
	}

	this.canUndo = function(){
		return undoList.length > 0;
	}

	this.redo = function(){

		if(self.canRedo()){
			var command = redoList.pop();

			command.do();

			undoList.push(command);
		}
	}

	this.canRedo = function(){
		return redoList.length > 0;
	}

	return this;
}

//########################
//### abstract command ###
//########################
historyPattern.AbstractCommand = function(){

	this._done = false;

	return this;
}

historyPattern.AbstractCommand.prototype.do = function(){

	if(!this._done){

		this._done = true;

		this._do();
	}
}

historyPattern.AbstractCommand.prototype.undo = function(){

	if(this._done){

		this._done = false;

		this._undo();
	}
}

historyPattern.AbstractCommand.prototype._do = function(){
	throw new Error("this method is abstract");
}

historyPattern.AbstractCommand.prototype._undo = function(){
	throw new Error("this method is abstract");
}
