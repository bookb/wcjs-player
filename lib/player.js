'use strict';

const {VlcPlayer} = require('./bindings');
const {EventEmitter} = require('events');
const {inherits} = require('util');

inherits(VlcPlayer, EventEmitter);

exports.VlcPlayer = VlcPlayer;