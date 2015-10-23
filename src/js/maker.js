/* 
 * Alvin IFTTT Control Application
 *
 * Copyright (c) 2015 James Fowler
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*global nvl, window, mConst,  clearTimeout, callWatchApp */
/*exported iftttMakerInterfaceFire, iftttMakerInterfaceTest */

/*
 * Do an action
 */
function iftttMakerInterfaceFire(num, turnon) {

  try {
    
    console.log("num = " + num + ", turnon = " + turnon);

    // Find out config information
    var key = nvl(window.localStorage.getItem("key"), "");

    // Escape if not configured, caller returns fail
    if (key === "") {
      window.localStorage.setItem("kvalid", "N");
      return false;
    }

    var eventName = "";
    var toggle = "N";
    switch (num) {
      case 0:
        eventName = nvl(window.localStorage.getItem("mn1"), "");
        toggle = nvl(window.localStorage.getItem("t1"), "N");
        break;
      case 1:
        eventName = nvl(window.localStorage.getItem("mn2"), "");
        toggle = nvl(window.localStorage.getItem("t2"), "N");
        break;
      case 2:
        eventName = nvl(window.localStorage.getItem("mn3"), "");
        toggle = nvl(window.localStorage.getItem("t3"), "N");
        break;
      case 3:
        eventName = nvl(window.localStorage.getItem("mn4"), "");
        toggle = nvl(window.localStorage.getItem("t4"), "N");
        break;
      case 4:
        eventName = nvl(window.localStorage.getItem("mn5"), "");
        toggle = nvl(window.localStorage.getItem("t5"), "N");
        break;
      case 5:
        eventName = nvl(window.localStorage.getItem("mn6"), "");
        toggle = nvl(window.localStorage.getItem("t6"), "N");
        break;
      case 6:
        eventName = nvl(window.localStorage.getItem("mn7"), "");
        toggle = nvl(window.localStorage.getItem("t7"), "N");
        break;
      case 7:
        eventName = nvl(window.localStorage.getItem("mn8"), "");
        toggle = nvl(window.localStorage.getItem("t8"), "N");
        break;
      case 8:
        eventName = nvl(window.localStorage.getItem("mn9"), "");
        toggle = nvl(window.localStorage.getItem("t9"), "N");
        break;
      case 9:
        eventName = nvl(window.localStorage.getItem("mn10"), "");
        toggle = nvl(window.localStorage.getItem("t10"), "N");
        break;
    }
    
    console.log("eventname = " + eventName);

    // menu name becomes event name, by lowercasing and replacing space with _
    // This saves the user having to put in and see two very similar strings
    eventName = eventName.toLowerCase().replace(/ /g, "_");

    // If this is a toggle then we suffix with _on or _off depending on which we
    // are doing
    if (toggle === "Y") {
      if (turnon === 1) {
        eventName += "_on";
      } else {
        eventName += "_off";
      }
    }
    
    console.log("doctored eventname = " + eventName);

    // Can't see a use for a payload
    var payload = {
      "value1" : "",
      "value2" : "",
      "value3" : ""
    };

    // Blast with an event
    var url = mConst().makerPrefix + eventName + mConst().makerSuffix + key;

    console.log("iftttMakerInterfaceFire: url=" + url);
    makeAjaxCall("POST", url, mConst().timeout, JSON.stringify(payload), function(resp) {
      console.log("iftttMakerInterfaceFire: " + JSON.stringify(resp));
      if (resp.status !== 1) {
        callWatchApp(mConst().ctrlRequestFail);
        window.localStorage.setItem("kvalid", "N");
      } else {
        callWatchApp(mConst().ctrlRequestOK);
        window.localStorage.setItem("kvalid", "Y");
      }
    });

  } catch (err) {
    callWatchApp(mConst().ctrlRequestFail);
    window.localStorage.setItem("kvalid", "N");
  }
  return true;
}

/*
 * Call the ifttt maker interface when the data export needs to be done
 */
function iftttMakerInterfaceTest() {

  try {

    // Find out config information
    var key = nvl(window.localStorage.getItem("key"), "");

    // Escape if not configured
    if (key === "") {
      console.log("ifttt maker deactivated");
      window.localStorage.setItem("kvalid", "N");
      return;
    }

    var payload = {
      "value1" : "",
      "value2" : "",
      "value3" : ""
    };

    var url = mConst().makerPrefix + "alvin_test" + mConst().makerSuffix + key;

    console.log("iftttMakerInterfaceTest: url=" + url);
    makeAjaxCall("POST", url, mConst().timeout, JSON.stringify(payload), function(resp) {
      console.log("iftttMakerInterfaceTest: " + JSON.stringify(resp));
      if (resp.status !== 1) {
        console.log("iftttMakerInterfaceTest: " + JSON.stringify(resp.errors));
        window.localStorage.setItem("kvalid", "N");
      } else {
        console.log("iftttMakerInterfaceTest:  OK");
        window.localStorage.setItem("kvalid", "Y");
      }
    });

  } catch (err) {
    console.log("iftttMakerInterfaceTest: " + err.message);
    window.localStorage.setItem("kvalid", "N");
  }
}

/*
 * Generic ajax support function (used by Maker and Hue bulb interface)
 */
function makeAjaxCall(mode, url, toTime, dataout, resp) {
  var tout = setTimeout(function() {
    resp({
      "status" : 0,
      "errors" : [ "timeout" ]
    });
  }, toTime);
  var req = new XMLHttpRequest();
  req.open(mode, url, true);
  req.setRequestHeader("Content-Type", "application/json");
  req.timeout = toTime;
  req.ontimeout = function() {
    resp({
      "status" : 0,
      "errors" : [ "timeout" ]
    });
    clearTimeout(tout);
  };
  req.onload = function() {
    if (req.readyState === 4 && req.status === 200) {
      clearTimeout(tout);
      resp({
        "status" : 1,
        "data" : safeJSONparse(req.responseText)
      });
    } else if (req.readyState === 4 && (req.status >= 300 && req.status <= 599)) {
      clearTimeout(tout);
      resp({
        "status" : 0,
        "errors" : [ req.status, nvl(req.responseText, "No Msg") ]
      });
    }
  };
  if (dataout === "") {
    req.send();
  } else {
    req.send(dataout);
  }

}

/*
 * JSON parse with built in safety - like if the message isn't bloody json
 */
function safeJSONparse(txt) {
  try {
    return JSON.parse(txt);
  } catch (err) {
    return txt;
  }
}
