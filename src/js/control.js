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

/*global window, nvl, mConst, iftttMakerInterfaceFire, iftttMakerInterfaceTest */

/*
 * Process ready from the watch
 */
Pebble.addEventListener("ready", function(e) {
  console.log("ready");
});

/*
 * Return a control value back ACK
 */
function callWatchApp(ctrlVal) {
  function decodeKeyCtrl(ctrlVal, keyVal, name) {
    return (ctrlVal & keyVal) ? name + " " : "";
  }
  console.log("ACK " + decodeKeyCtrl(ctrlVal, mConst().ctrlVersionDone, "ctrlVersionDone") + decodeKeyCtrl(ctrlVal, mConst().ctrlGotRequest, "ctrlGotRequest") + decodeKeyCtrl(ctrlVal, mConst().ctrlRequestOK, "ctrlRequestOK") + decodeKeyCtrl(ctrlVal, mConst().ctrlRequestFail, "ctrlRequestFail"));
  Pebble.sendAppMessage({
    "ctrl" : ctrlVal
  });
}

/*
 * Process sample from the watch
 */
Pebble.addEventListener("appmessage", function(e) {

  // Incoming version number
  if (typeof e.payload.version !== "undefined") {
    var version = parseInt(e.payload.version, 10);
    console.log("MSG version=" + version);
    window.localStorage.setItem("version", version);
    callWatchApp(mConst().ctrlVersionDone);
  }

  // Incoming request
  if (typeof e.payload.fire !== "undefined") {
    var fire = parseInt(e.payload.fire, 10);
    var turnon = parseInt(e.payload.turnon, 10);
    console.log("MSG fire=" + fire + ", turnon=" + turnon);
    if (iftttMakerInterfaceFire(fire, turnon)) {
      callWatchApp(mConst().ctrlGotRequest);
    } else {
      callWatchApp(mConst().ctrlRequestFail);
    }
  }

});

/*
 * Monitor the closing of the config/display screen so as we can do a save if
 * needed
 */
Pebble.addEventListener("webviewclosed", function(e) {
  console.log("webviewclosed " + e.response);

  // Nothing returned
  if (e.response === null || typeof e.response === 'undefined') {
    console.log("no config returned");
    return;
  }

  // Even then a lack of trust is reasonable
  var configData;
  try {
    configData = JSON.parse(decodeURIComponent(e.response));
  } catch (err) {
    console.log("no config returned");
    return;
  }

  // Process
  if (configData.action === "save") {
    window.localStorage.setItem("key", configData.key);
    window.localStorage.setItem("mn1", configData.mn1);
    window.localStorage.setItem("mn2", configData.mn2);
    window.localStorage.setItem("mn3", configData.mn3);
    window.localStorage.setItem("mn4", configData.mn4);
    window.localStorage.setItem("mn5", configData.mn5);
    window.localStorage.setItem("mn6", configData.mn6);
    window.localStorage.setItem("mn7", configData.mn7);
    window.localStorage.setItem("mn8", configData.mn8);
    window.localStorage.setItem("mn9", configData.mn9);
    window.localStorage.setItem("mn10", configData.mn10);
    window.localStorage.setItem("t1", configData.t1);
    window.localStorage.setItem("t2", configData.t2);
    window.localStorage.setItem("t3", configData.t3);
    window.localStorage.setItem("t4", configData.t4);
    window.localStorage.setItem("t5", configData.t5);
    window.localStorage.setItem("t6", configData.t6);
    window.localStorage.setItem("t7", configData.t7);
    window.localStorage.setItem("t8", configData.t8);
    window.localStorage.setItem("t9", configData.t9);
    window.localStorage.setItem("t10", configData.t10);
    window.localStorage.setItem("kvalid", configData.keyValid);

    console.log("Test settings requested");
    iftttMakerInterfaceTest();

    console.log("Tell the watch what it needs to know");

    var toggle = (configData.t1 === "Y" ? 1 : 0) | (configData.t2 === "Y" ? 2 : 0) | (configData.t3 === "Y" ? 4 : 0) | (configData.t4 === "Y" ? 8 : 0) | (configData.t5 === "Y" ? 16 : 0) | (configData.t6 === "Y" ? 32 : 0) | (configData.t7 === "Y" ? 64 : 0) | (configData.t8 === "Y" ? 128 : 0) | (configData.t9 === "Y" ? 256 : 0) | (configData.t10 === "Y" ? 512 : 0);

    Pebble.sendAppMessage({
      "menuName1" : configData.mn1,
      "menuName2" : configData.mn2,
      "menuName3" : configData.mn3,
      "menuName4" : configData.mn4,
      "menuName5" : configData.mn5,
      "menuName6" : configData.mn6,
      "menuName7" : configData.mn7,
      "menuName8" : configData.mn8,
      "menuName9" : configData.mn9,
      "menuName10" : configData.mn10,
      "menuToggle" : toggle
    });

  }
});

/*
 * Build the url for the config and report display @param noset
 */
function buildUrl() {
  // If the version is set, keep it, if not the provide a not ready screen
  var version = nvl(window.localStorage.getItem("version"), mConst().versionDef);
  if (parseInt(version, 10) < mConst().lowestVersion) {
    return mConst().urlNotReady;
  }

  var key = encodeURIComponent(nvl(window.localStorage.getItem("key"), ""));
  var mn1 = encodeURIComponent(nvl(window.localStorage.getItem("mn1"), ""));
  var mn2 = encodeURIComponent(nvl(window.localStorage.getItem("mn2"), ""));
  var mn3 = encodeURIComponent(nvl(window.localStorage.getItem("mn3"), ""));
  var mn4 = encodeURIComponent(nvl(window.localStorage.getItem("mn4"), ""));
  var mn5 = encodeURIComponent(nvl(window.localStorage.getItem("mn5"), ""));
  var mn6 = encodeURIComponent(nvl(window.localStorage.getItem("mn6"), ""));
  var mn7 = encodeURIComponent(nvl(window.localStorage.getItem("mn7"), ""));
  var mn8 = encodeURIComponent(nvl(window.localStorage.getItem("mn8"), ""));
  var mn9 = encodeURIComponent(nvl(window.localStorage.getItem("mn9"), ""));
  var mn10 = encodeURIComponent(nvl(window.localStorage.getItem("mn10"), ""));
  var t1 = nvl(window.localStorage.getItem("t1"), "N");
  var t2 = nvl(window.localStorage.getItem("t2"), "N");
  var t3 = nvl(window.localStorage.getItem("t3"), "N");
  var t4 = nvl(window.localStorage.getItem("t4"), "N");
  var t5 = nvl(window.localStorage.getItem("t5"), "N");
  var t6 = nvl(window.localStorage.getItem("t6"), "N");
  var t7 = nvl(window.localStorage.getItem("t7"), "N");
  var t8 = nvl(window.localStorage.getItem("t8"), "N");
  var t9 = nvl(window.localStorage.getItem("t9"), "N");
  var t10 = nvl(window.localStorage.getItem("t10"), "N");
  var kvalid = nvl(window.localStorage.getItem("kvalid"), "X");

  var url = mConst().url + version + ".html?" + "vers=" + version + "&" + "key=" + key + "&" + "mn1=" + mn1 + "&" + "mn2=" + mn2 + "&" + "mn3=" + mn3 + "&" + "mn4=" + mn4 + "&" + "mn5=" + mn5 + "&" + "mn6=" + mn6 + "&" + "mn7=" + mn7 + "&" + "mn8=" + mn8 + "&" + "mn9=" + mn9 + "&" + "mn10=" + mn10 + "&" + "t1=" + t1 + "&" + "t2=" + t2 + "&" + "t3=" + t3 + "&" + "t4=" + t4 + "&" + "t5=" + t5 + "&" + "t6=" + t6 + "&" + "t7=" + t7 + "&" + "t8=" + t8 + "&" + "t9=" + t9 + "&" + "t10=" + t10 + "&kvalid=" + kvalid;

  console.log("url=" + url + " (len=" + url.length + ")");
  return url;
}

/*
 * Show the config/display page - this will show a graph and allow a reset
 */
Pebble.addEventListener("showConfiguration", function(e) {
  Pebble.openURL(buildUrl());
});

/*
 * Still unclear if this serves a purpose at all
 */
Pebble.addEventListener("configurationClosed", function(e) {
  console.log("configurationClosed");
});
