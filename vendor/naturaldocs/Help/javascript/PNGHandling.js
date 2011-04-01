// Parts derived from:
//    Opacity Displayer, Version 1.0
//    Copyright Michael Lovitt, 6/2002.
//    Distribute freely, but please leave this notice intact.
//    http://www.alistapart.com/articles/pngopacity/

// Parts derived from:
//    Natural Docs
//    Copyright (C) 2003-2004 Greg Valure
//    http://www.naturaldocs.org/


var pngTransform;
var pngNormal;

var agt=navigator.userAgent.toLowerCase();

if (agt.indexOf("opera") != -1)
    {
    if ( (agt.indexOf("opera 5") != -1 || agt.indexOf("opera/5") != -1) &&
         agt.indexOf("mac") != -1)
        {
        pngNormal = 1;
        }
    else if (agt.indexOf("opera 6") != -1 || agt.indexOf("opera/6") != -1 ||
               agt.indexOf("opera 7") != -1 || agt.indexOf("opera/7") != -1)
        {
        pngNormal = 1;
        }
    }

else if (agt.indexOf("msie") != -1)
    {
    if (agt.indexOf("msie 5.5") != -1 || agt.indexOf("msie 6") != -1)
        {
        if (agt.indexOf("mac") != -1)
            {  pngNormal = 1;  }
        else if (agt.indexOf("win") != -1)
            {  pngTransform = 1;  };
        }

    else if (agt.indexOf("msie 5") != -1)
        {
        if (agt.indexOf("mac") != -1)
            {  pngNormal = 1;  };
        }

    else if (agt.indexOf("msie 7") != -1)
        {  pngNormal = 1;  }
    }

else if (agt.indexOf("gecko") != -1)
    {
    pngNormal = 1;
    }


function PNGGIF(strPath, intWidth, intHeight, strAlt, strID)
    {
    if (pngTransform)
        {
        document.write('<div style="height:'+intHeight+'px;width:'+intWidth+'px;filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(src=\''+strPath+'.png\', sizingMethod=\'scale\')" id="'+strID+'"></div>');
        }
    else if (pngNormal)
        {
        document.write('<img src="'+strPath+'.png" width="'+intWidth+'" height="'+intHeight+'" alt="'+strAlt+'" id="'+strID+'"/>');
        }
    else
        {
        document.write('<img src="'+strPath+'.gif" width="'+intWidth+'" height="'+intHeight+'" alt="'+strAlt+'" id="'+strID+'" />');
        }
    };
