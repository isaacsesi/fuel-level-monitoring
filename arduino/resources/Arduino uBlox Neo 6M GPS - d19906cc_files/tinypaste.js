$.fn.qtip.styles.tinypaste = { // Last part is the name of the style
   width: 200,
   background: '#111111',
   color: 'white',
   border: {
      width: 7,
      radius: 5,
      color: '#111111'
   },
   tip: 'bottomLeft',
   name: 'dark' // Inherit the rest of the attributes from the preset dark style
}

      
$.fn.extend({
	tinytip: function(content, target, tooltip, width)
	{
		target = (typeof target == 'string') ? target : "topMiddle";
		tooltip = (typeof tooltip == 'string') ? tooltip : "bottomMiddle";
		width = (typeof width == 'string') ? width : 200;
		
		var obj = this;
		obj.qtip({
		   content: content,
		   show: 'mouseover',
		   hide: { fixed: true },
		   position: { corner: { target: target, tooltip: tooltip } },
		   style: { width: width, tip: true, name: 'tinypaste', border: { width: 2, radius: 5 } }
		});
        }
});
        
        
        
function socialNetwork(paste)
{
	var html = "<div class='bold blue font15'>Share Paste #"+paste+":</div><br /><div class='center'>";
	html += "<a href='#' class='linkButton' onclick=\"return socialNetworkTwitter('"+paste+"');\"><img src='"+root_url+"public/images/twitter.png' alt='' /> Twitter</a> &nbsp; ";
	html += "<a href='#' class='linkButton' onclick=\"return socialNetworkFacebook('"+paste+"');\"><img src='"+root_url+"public/images/facebook.png' alt='' /> Facebook</a> &nbsp; ";
	html += "<a href='#' class='linkButton' onclick=\"return socialNetworkOther('"+paste+"');\"><img src='"+root_url+"public/images/help.png' alt='' /> Other</a></div>";
	$.facebox(html);
}


function socialNetworkTwitter(paste)
{
	popPreview('http://twitter.com/home?status='+escape(root_url+paste+" - #tinypaste"));
	return false;
}

function socialNetworkFacebook(paste)
{
	popPreview('http://www.facebook.com/sharer.php?u='+escape(root_url+paste)+'&t='+escape(paste));
	return false;
}

function socialNetworkOther(paste)
{
	var html = "<div class='blue font14 bold'>Here is a URL to the paste:<br /><br /><textarea cols='50' rows='1'>"+root_url+paste+"</textarea><br /><br />Send it to you friends to share.</div>";
	
	$.facebox(html);
	return false;
}


function popPreview(url)
{
	var preview = window.open(url, 'preview', 'toolbar=0,scrollbars=1,location=0,statusbar=0,menubar=0,resizable=1,width=900,height=700');
	return false;
}



function embedCode(paste)
{
	var url = (typeof paste == 'string' ? "?id="+paste : '');
	if(url == '')
	{
		return;
	}
	var html = "<div class='bold blue font15'>Embed TinyPaste</div><br /><div>Copy and paste this code into your site's source (JavaScript Required):</div><br />";
	html += "<div class='font11'>&lt;script type='text/javascript' src='"+root_url+"embed.js"+url+"&height=300&width=700'&gt;&lt;/script&gt;</div>";
	$.facebox(html);
}

function getARealBrowser()
{
	if($.browser.msie)
	{
		$('div.rounded10').wrapInner("<div class='roundedinner'></div>");
		$('div.rounded10 div.roundedinner').before("<div class='roundedtl'></div><div class='roundedtop'></div><div class='roundedtr'></div>");
		$('div.rounded10 div.roundedinner').after("<div class='roundedbl'></div><div class='roundedbottom'></div><div class='roundedbr'></div>");
		$('div.rounded10').addClass('roundfix');
	}
}

$(document).ready(getARealBrowser);