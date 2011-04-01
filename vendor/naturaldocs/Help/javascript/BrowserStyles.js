
//
//  Browser Styles
// ____________________________________________________________________________

var agt=navigator.userAgent.toLowerCase();
var browserType;
var browserVer;

if (agt.indexOf("opera") != -1)
    {
    browserType = "Opera";

    if (agt.indexOf("opera 5") != -1 || agt.indexOf("opera/5") != -1)
        {  browserVer = "Opera5";  }
    else if (agt.indexOf("opera 6") != -1 || agt.indexOf("opera/6") != -1)
        {  browserVer = "Opera6";  }
    else if (agt.indexOf("opera 7") != -1 || agt.indexOf("opera/7") != -1)
        {  browserVer = "Opera7";  }
    }

else if (agt.indexOf("khtml") != -1 || agt.indexOf("konq") != -1 || agt.indexOf("safari") != -1)
    {
    browserType = "KHTML";
    }

else if (agt.indexOf("msie") != -1)
    {
    browserType = "IE";

    if (agt.indexOf("msie 4") != -1)
        {  browserVer = "IE4";  }
    else if (agt.indexOf("msie 5") != -1)
        {  browserVer = "IE5";  }
    else if (agt.indexOf("msie 6") != -1)
        {  browserVer = "IE6";  }
    else if (agt.indexOf("msie 7") != -1)
        {  browserVer = "IE7";  }
    }

else if (agt.indexOf("gecko") != -1)
    {
    browserType = "Gecko";
    }

// Opera already taken care of.
else if (agt.indexOf("mozilla") != -1 && agt.indexOf("compatible") == -1 && agt.indexOf("spoofer") == -1 &&
           agt.indexOf("webtv") == -1 && agt.indexOf("hotjava") == -1)
    {
    browserType = "Netscape";

    if (agt.indexOf("mozilla/4") != -1)
        {  browserVer = "Netscape4";  }
    }


function OpeningBrowserTags()
    {
    if (browserType)
        {
        document.write('<div class='+browserType+'>');

        if (browserVer)
            {  document.write('<div class='+browserVer+'>');  }
        }
    };

function ClosingBrowserTags()
    {
    if (browserType)
        {
        document.write('</div>');

        if (browserVer)
            {  document.write('</div>');  }
        }
    };
