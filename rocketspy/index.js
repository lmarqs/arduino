"use strict";

const WEB_SERVER_HOST = "192.168.4.1";

class VideoStreamFromWebServer {
  constructor(el, host) {
    this.host = host;
    this.el = el;
  }

  begin() {
    this.el.src = `http://${this.host}:81/stream`;
  }
}

class InputWebSocket {
  constructor(host) {
    this.host = host;
    this.ws = null;
  }

  begin() {
    this.ws = new WebSocket(`ws://${this.host}/input`);
  }

  send(wheelChair) {
    if (this.ws && this.ws.readyState === WebSocket.OPEN) {
      const buffer = new Uint8Array([
        wheelChair.leftSpeed,
        wheelChair.rightSpeed,
      ]);

      this.ws.send(buffer);
    }
  }
}

class WheelChair {
  constructor(el) {
    this.leftSlider = document.createElement("input");
    this.rightSlider = document.createElement("input");
    this.leftSpeed = 0;
    this.rightSpeed = 0;

    el.appendChild(this.leftSlider);
    el.appendChild(this.rightSlider);
  }

  begin() {
    this.leftSlider.type = "range";
    this.leftSlider.readOnly = true;
    this.leftSlider.max = 100;
    this.leftSlider.min = -100;
    this.rightSlider.type = "range";
    this.rightSlider.readOnly = true;
    this.rightSlider.max = 100;
    this.rightSlider.min = -100;
    this.render();
  }

  move(leftSpeed, rightSpeed) {
    const acc = 10;

    this.leftSpeed += Math.max(-acc, Math.min(Math.round(leftSpeed - this.leftSpeed), acc));
    this.rightSpeed += Math.max(-acc, Math.min(Math.round(rightSpeed - this.rightSpeed), acc));

    this.render();
  }

  render() {
    this.leftSlider.value = this.leftSpeed;
    this.rightSlider.value = this.rightSpeed;
  }
}

const joystick = new JoyStick(document.getElementById("joystick"));

const stream = new VideoStreamFromWebServer(
  document.getElementById("stream"),
  WEB_SERVER_HOST,
);

const inputWebSocket = new InputWebSocket(WEB_SERVER_HOST);

const wheelChair = new WheelChair(document.getElementById("wheel-chair"));

function main() {
  stream.begin();
  joystick.begin();
  inputWebSocket.begin();
  wheelChair.begin();
}

function loop() {
  const [leftSpeed, rightSpeed] = getSpeedFromUserInput();

  wheelChair.move(leftSpeed, rightSpeed);

  inputWebSocket.send(wheelChair);
}

; (() => {
  main();
  setInterval(loop, 50);
})();


function getSpeedFromUserInput() {
  const gamepad = navigator.getGamepads().filter(Boolean)[0];

  if (gamepad) {
    return getSpeedFromGamepad(gamepad);
  }

  return [0, 0];
}

function getSpeedFromGamepad(gamepad) {
  let [leftStickX, leftStickY, rightStickX, rightStickY] = gamepad.axes.map(axis => Math.round(axis * 100));

  const [a, b, x, y, l1, r1, l2, r2, select, start, l3, r3, up, down, left, right] = gamepad.buttons;

  if (l1.pressed) {
    return [-30, 30];
  }

  if (r1.pressed) {
    return [30, -30];
  }

  if (a.pressed) {
    rightStickY = -100;
  }

  if (b.pressed) {
    rightStickY = 100;
  }

  return [
    -rightStickY * (100 + Math.min(0, leftStickX)) / 100,
    -rightStickY * (100 - Math.max(0, leftStickX)) / 100
  ];
}

function convertAxesToSpeed(x, y) {
  const speed = Math.sqrt(x * x + y * y);
  const angle = Math.atan2(y, x);

  const speedL = speed * Math.cos(angle - Math.PI / 4);
  const speedR = speed * Math.sin(angle - Math.PI / 4);

  return [speedL, speedR];
}

function throttle(fn, waitTime) {
  let timer = null
  let lastExec = null

  return function () {
    const context = this
    const args = arguments

    if (!lastExec) {
      fn.apply(context, args)
      lastExec = Date.now()
    } else {
      clearTimeout(timer)
      timer = setTimeout(function () {
        if ((Date.now() - lastExec) >= waitTime) {
          fn.apply(context, args)
          lastExec = Date.now()
        }
      }, waitTime - (Date.now() - lastExec))
    }
  }
}

/*
 * Name          : joy.js
 * @author       : Roberto D'Amico (Bobboteck)
 */
function JoyStick(container, parameters, callback) {
  let state = {
    xPosition: 0,
    yPosition: 0,
    x: 0,
    y: 0,
    cardinalDirection: "C"
  };


  parameters = parameters || {};

  var width = (typeof parameters.width === "undefined" ? 0 : parameters.width),
    height = (typeof parameters.height === "undefined" ? 0 : parameters.height),
    internalFillColor = (typeof parameters.internalFillColor === "undefined" ? "#00AA00" : parameters.internalFillColor),
    internalLineWidth = (typeof parameters.internalLineWidth === "undefined" ? 2 : parameters.internalLineWidth),
    internalStrokeColor = (typeof parameters.internalStrokeColor === "undefined" ? "#003300" : parameters.internalStrokeColor),
    externalLineWidth = (typeof parameters.externalLineWidth === "undefined" ? 2 : parameters.externalLineWidth),
    externalStrokeColor = (typeof parameters.externalStrokeColor === "undefined" ? "#008000" : parameters.externalStrokeColor),
    autoReturnToCenter = (typeof parameters.autoReturnToCenter === "undefined" ? true : parameters.autoReturnToCenter);

  callback = callback || function (state) { };

  var canvas = document.createElement("canvas");

  var context = canvas.getContext("2d");

  var pressed,
    circumference,
    internalRadius,
    maxMoveStick,
    externalRadius,
    centerX,
    centerY,
    directionHorizontalLimitPos,
    directionHorizontalLimitNeg,
    directionVerticalLimitPos,
    directionVerticalLimitNeg,
    movedX,
    movedY;

  // Check if the device support the touch or not
  if ("ontouchstart" in document.documentElement) {
    canvas.addEventListener("touchstart", onTouchStart, false);
    document.addEventListener("touchmove", onTouchMove, false);
    document.addEventListener("touchend", onTouchEnd, false);
  }
  else {
    canvas.addEventListener("mousedown", onMouseDown, false);
    document.addEventListener("mousemove", onMouseMove, false);
    document.addEventListener("mouseup", onMouseUp, false);
  }

  /******************************************************
   * Private methods
   *****************************************************/

  /**
   * @desc Draw the external circle used as reference position
   */
  function drawExternal() {
    context.beginPath();
    context.arc(centerX, centerY, externalRadius, 0, circumference, false);
    context.lineWidth = externalLineWidth;
    context.strokeStyle = externalStrokeColor;
    context.stroke();
  }

  /**
   * @desc Draw the internal stick in the current position the user have moved it
   */
  function drawInternal() {
    context.beginPath();
    if (movedX < internalRadius) { movedX = maxMoveStick; }
    if ((movedX + internalRadius) > canvas.width) { movedX = canvas.width - (maxMoveStick); }
    if (movedY < internalRadius) { movedY = maxMoveStick; }
    if ((movedY + internalRadius) > canvas.height) { movedY = canvas.height - (maxMoveStick); }
    context.arc(movedX, movedY, internalRadius, 0, circumference, false);
    // create radial gradient
    var grd = context.createRadialGradient(centerX, centerY, 5, centerX, centerY, 200);
    // Light color
    grd.addColorStop(0, internalFillColor);
    // Dark color
    grd.addColorStop(1, internalStrokeColor);
    context.fillStyle = grd;
    context.fill();
    context.lineWidth = internalLineWidth;
    context.strokeStyle = internalStrokeColor;
    context.stroke();
  }

  /**
   * @desc Events for manage touch
   */
  let touchId = null;
  function onTouchStart(event) {
    pressed = true;
    touchId = event.targetTouches[0].identifier;
  }

  function onTouchMove(event) {
    if (pressed && event.targetTouches[0].target === canvas) {
      movedX = event.targetTouches[0].pageX;
      movedY = event.targetTouches[0].pageY;
      // Manage offset
      if (canvas.offsetParent.tagName.toUpperCase() === "BODY") {
        movedX -= canvas.offsetLeft;
        movedY -= canvas.offsetTop;
      } else {
        movedX -= canvas.offsetParent.offsetLeft;
        movedY -= canvas.offsetParent.offsetTop;
      }
      // Delete canvas
      context.clearRect(0, 0, canvas.width, canvas.height);
      // Redraw object
      drawExternal();
      drawInternal();

      // Set attribute of callback
      state.xPosition = movedX;
      state.yPosition = movedY;
      state.x = (100 * ((movedX - centerX) / maxMoveStick)).toFixed();
      state.y = ((100 * ((movedY - centerY) / maxMoveStick)) * -1).toFixed();
      state.cardinalDirection = getCardinalDirection();
      callback(state);
    }
  }

  function onTouchEnd(event) {
    if (event.changedTouches[0].identifier !== touchId) return;

    pressed = false;
    // If required reset position store variable
    if (autoReturnToCenter) {
      movedX = centerX;
      movedY = centerY;
    }
    // Delete canvas
    context.clearRect(0, 0, canvas.width, canvas.height);
    // Redraw object
    drawExternal();
    drawInternal();

    // Set attribute of callback
    state.xPosition = movedX;
    state.yPosition = movedY;
    state.x = (100 * ((movedX - centerX) / maxMoveStick)).toFixed();
    state.y = ((100 * ((movedY - centerY) / maxMoveStick)) * -1).toFixed();
    state.cardinalDirection = getCardinalDirection();

    callback(state);
  }

  /**
   * @desc Events for manage mouse
   */
  function onMouseDown(event) {
    pressed = true;
  }

  /* To simplify this code there was a new experimental feature here: https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent/offsetX , but it present only in Mouse case not metod presents in Touch case :-( */
  function onMouseMove(event) {
    if (pressed) {
      movedX = event.pageX;
      movedY = event.pageY;
      // Manage offset
      if (canvas.offsetParent.tagName.toUpperCase() === "BODY") {
        movedX -= canvas.offsetLeft;
        movedY -= canvas.offsetTop;
      }
      else {
        movedX -= canvas.offsetParent.offsetLeft;
        movedY -= canvas.offsetParent.offsetTop;
      }
      // Delete canvas
      context.clearRect(0, 0, canvas.width, canvas.height);
      // Redraw object
      drawExternal();
      drawInternal();

      // Set attribute of callback
      state.xPosition = movedX;
      state.yPosition = movedY;
      state.x = (100 * ((movedX - centerX) / maxMoveStick)).toFixed();
      state.y = ((100 * ((movedY - centerY) / maxMoveStick)) * -1).toFixed();
      state.cardinalDirection = getCardinalDirection();
      callback(state);
    }
  }

  function onMouseUp(event) {
    pressed = false;
    // If required reset position store variable
    if (autoReturnToCenter) {
      movedX = centerX;
      movedY = centerY;
    }
    // Delete canvas
    context.clearRect(0, 0, canvas.width, canvas.height);
    // Redraw object
    drawExternal();
    drawInternal();

    // Set attribute of callback
    state.xPosition = movedX;
    state.yPosition = movedY;
    state.x = (100 * ((movedX - centerX) / maxMoveStick)).toFixed();
    state.y = ((100 * ((movedY - centerY) / maxMoveStick)) * -1).toFixed();
    state.cardinalDirection = getCardinalDirection();
    callback(state);
  }

  function getCardinalDirection() {
    let result = "";
    let orizontal = movedX - centerX;
    let vertical = movedY - centerY;

    if (vertical >= directionVerticalLimitNeg && vertical <= directionVerticalLimitPos) {
      result = "C";
    }
    if (vertical < directionVerticalLimitNeg) {
      result = "N";
    }
    if (vertical > directionVerticalLimitPos) {
      result = "S";
    }

    if (orizontal < directionHorizontalLimitNeg) {
      if (result === "C") {
        result = "W";
      }
      else {
        result += "W";
      }
    }
    if (orizontal > directionHorizontalLimitPos) {
      if (result === "C") {
        result = "E";
      }
      else {
        result += "E";
      }
    }

    return result;
  }

  /******************************************************
   * Public methods
   *****************************************************/

  this.begin = function () {
    // Fixing Unable to preventDefault inside passive event listener due to target being treated as passive in Chrome [Thanks to https://github.com/artisticfox8 for this suggestion]
    container.style.touchAction = "none";

    container.appendChild(canvas);

    if (!width) {
      width = container.clientWidth;
    }

    if (!height) {
      height = container.clientHeight;
    }

    canvas.width = width;
    canvas.height = height;

    pressed = false;
    circumference = 2 * Math.PI;
    internalRadius = (canvas.width - ((canvas.width / 2) + 10)) / 2;
    maxMoveStick = internalRadius + 5;
    externalRadius = internalRadius + 30;
    centerX = canvas.width / 2;
    centerY = canvas.height / 2;
    directionHorizontalLimitPos = canvas.width / 10;
    directionHorizontalLimitNeg = directionHorizontalLimitPos * -1;
    directionVerticalLimitPos = canvas.height / 10;
    directionVerticalLimitNeg = directionVerticalLimitPos * -1;

    movedX = centerX;
    movedY = centerX;

    drawExternal();
    drawInternal();
  }

  /**
   * @desc The width of canvas
   * @return Number of pixel width 
   */
  this.getWidth = function () {
    return canvas.width;
  };

  /**
   * @desc The height of canvas
   * @return Number of pixel height
   */
  this.getHeight = function () {
    return canvas.height;
  };

  /**
   * @desc The X position of the cursor relative to the canvas that contains it and to its dimensions
   * @return Number that indicate relative position
   */
  this.getPosX = function () {
    return movedX;
  };

  /**
   * @desc The Y position of the cursor relative to the canvas that contains it and to its dimensions
   * @return Number that indicate relative position
   */
  this.getPosY = function () {
    return movedY;
  };

  /**
   * @desc Normalizzed value of X move of stick
   * @return Integer from -100 to +100
   */
  this.getX = function () {
    return (100 * ((movedX - centerX) / maxMoveStick)).toFixed();
  };

  /**
   * @desc Normalizzed value of Y move of stick
   * @return Integer from -100 to +100
   */
  this.getY = function () {
    return ((100 * ((movedY - centerY) / maxMoveStick)) * -1).toFixed();
  };

  /**
   * @desc get the direction of the cursor as a string that indicates the cardinal points where this is oriented
   * @return String of cardinal point N, NE, E, SE, S, SW, W, NW and C when it is placed in the center
   */
  this.getDir = function () {
    return getCardinalDirection();
  };
}
