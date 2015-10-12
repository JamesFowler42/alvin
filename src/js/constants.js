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

/*exported mConst */

/*
 * Constants
 */
function mConst() {
  var urlPrefix = "http://ui.morpheuz.net/alvin/";
  return {
    limit : 60,
    divisor : 600000,
    url : urlPrefix + "config-",
    versionDef : "0",
    ctrlVersionDone : 1,
    ctrlGotRequest : 2,
    ctrlRequestOK : 4,
    ctrlRequestFail : 8,
    timeout : 4000,
    urlNotReady : urlPrefix + "config-not-ready.html",
    makerPrefix : "https://maker.ifttt.com/trigger/",
    makerSuffix : "/with/key/"
  };
}
