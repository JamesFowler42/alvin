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

/*global nvl, window, mLang, makeAjaxCall, mConst, buildUrl, generateCopyLinkData */
/*exported iftttMakerInterfaceAlarm, iftttMakerInterfaceData, iftttMakerInterfaceBedtime */

/*
 * Call the ifttt maker interface when the alarm sounds
 */
function iftttMakerInterfaceAlarm() {

  try {
  
    // Find out config information
    var ifkey =  nvl(window.localStorage.getItem("ifkey"), "");
  
    // Escape if not configured
    if (ifkey === "") {
      console.log("ifttt maker deactivated");
      window.localStorage.setItem("ifstat", mLang().disabled);
      return;
    }
  
    var payload = { "value1" : "", "value2" : "", "value3" : "" };

    var url = mConst().makerAlarmUrl + ifkey;
    
    console.log("iftttMakerInterfaceAlarm: url=" + url);
    window.localStorage.setItem("ifstat", mLang().sending);
    makeAjaxCall("POST", url, mConst().timeout, JSON.stringify(payload), function(resp) {
      console.log("iftttMakerInterfaceAlarm: " + JSON.stringify(resp));
      if (resp.status !== 1) {
        window.localStorage.setItem("ifstat", JSON.stringify(resp.errors));
      } else {
        window.localStorage.setItem("ifstat", mLang().ok);
      }     
    });
    
  } catch (err) {
    window.localStorage.setItem("ifstat", err.message);
  }
}

/*
 * Call the ifttt maker interface when the data export needs to be done
 */
function iftttMakerInterfaceData() {

  try {
  
    // Find out config information
    var ifkey =  nvl(window.localStorage.getItem("ifkey"), "");
  
    // Escape if not configured
    if (ifkey === "") {
      console.log("ifttt maker deactivated");
      window.localStorage.setItem("ifstat", mLang().disabled);
      return;
    }
    
    var base = window.localStorage.getItem("base");
    var resetDate = new Date(parseInt(base, 10)).format(mConst().displayDateFmt);
    var urlToAttach = buildUrl("Y");
    var csvData = generateCopyLinkData();
  
    var payload = { "value1" : resetDate, "value2" : urlToAttach, "value3" : csvData };
  
    var url = mConst().makerDataUrl + ifkey;
    
    console.log("iftttMakerInterfaceData: url=" + url);
    window.localStorage.setItem("ifstat", mLang().sending);
    makeAjaxCall("POST", url, mConst().timeout, JSON.stringify(payload), function(resp) {
      console.log("iftttMakerInterfaceData: " + JSON.stringify(resp));
      if (resp.status !== 1) {
        window.localStorage.setItem("ifstat", JSON.stringify(resp.errors));
      } else {
        window.localStorage.setItem("ifstat", mLang().ok);
      }     
    });
    
  } catch (err) {
    window.localStorage.setItem("ifstat", err.message);
  }
}

/*
 * Call the ifttt maker interface when the bedtime is activated
 */
function iftttMakerInterfaceBedtime() {

  try {
  
    // Find out config information
    var ifkey =  nvl(window.localStorage.getItem("ifkey"), "");
  
    // Escape if not configured
    if (ifkey === "") {
      console.log("ifttt maker deactivated");
      window.localStorage.setItem("ifstat", mLang().disabled);
      return;
    }
  
    var payload = { "value1" : "", "value2" : "", "value3" : "" };

    var url = mConst().makerBedtimeUrl + ifkey;
    
    console.log("iftttMakerInterfaceBedtime: url=" + url);
    window.localStorage.setItem("ifstat", mLang().sending);
    makeAjaxCall("POST", url, mConst().timeout, JSON.stringify(payload), function(resp) {
      console.log("iftttMakerInterfaceBedtime: " + JSON.stringify(resp));
      if (resp.status !== 1) {
        window.localStorage.setItem("ifstat", JSON.stringify(resp.errors));
      } else {
        window.localStorage.setItem("ifstat", mLang().ok);
      }     
    });
    
  } catch (err) {
    window.localStorage.setItem("ifstat", err.message);
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

