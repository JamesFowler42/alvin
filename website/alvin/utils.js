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

/*
 * Extract parameters from URL
 */
function getParameterByName(name) {
  name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
  var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"), results = regex.exec(location.search);
  return results == null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}

/*
 * Set the on screen version warning text if the version is non-current
 */
function setScreenMessageBasedOnVersion(vers) {
  $(".versproblem").show();
  $.ajaxSetup({ scriptCharset: "utf-8" , contentType: "application/json; charset=utf-8"});
  $.getJSON("currentversion.json?v=" + new Date().getTime(), function(data) {
    if (typeof data !== "undefined" && typeof data.version !== "undefined") {
      var currentVer = parseInt(data.version, 10);
      var requestVer = parseInt(vers, 10);
      $(".versproblem").hide();
      if (currentVer > requestVer) {
        $(".verswarning").show();
      } else if (currentVer < requestVer) {
        $(".versbeta").show();
      }
    }
  }).error(function(args) {
    $(".versproblem").text("Error attempting to find the current version: " + JSON.stringify(args));
  });
}

/*
 * Adjust for viewport
 */
function scaleToViewport() {

  // Get the width and limit to 320px to 800px
  var viewportWidth = $(window).width();
  if (viewportWidth > 800) {
    viewportWidth = 800;
  } else if (viewportWidth < 320) {
    viewportWidth = 320;
  }
  viewportWidth -= 5;

  // Set any element that has requested it to the required width
  $(".vpwidth").width(viewportWidth);
  $(".vpheight").height(viewportWidth * 250 / 318);

}

/*
 * Setup to auto adjust for viewport
 */
function adjustForViewport() {
  scaleToViewport();
  $(window).resize(scaleToViewport);
}

/*
 * Trim safely
 */
function safeTrim(strval) {
  try {
    return strval.trim();
  } catch (err) {
    return strval;
  }
}

/*
 * Protection against nulls
 */
function nvl(field, defval) {
  return field === null || field === "null" || typeof field === "undefined" ? defval : field;
}

