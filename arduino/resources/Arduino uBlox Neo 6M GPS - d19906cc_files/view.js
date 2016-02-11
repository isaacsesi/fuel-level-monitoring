function clickAd(paste, location)
{
	$(document).ready(function()
	{
		$.post(root_url + "api/tinypaste/getpaid_ad.php", { clicked: paste }, function()
		{
			if(typeof location == 'string')
			{
				window.location = location;
			}
		});
	});
}


function calcHeight(height)
{
	if(parseInt(height) <= 300)
	{
		height = "300px";
	}
	
	var docHeight = ($(document).height() - 550);
	
	if(parseInt(docHeight) < 300) { docHeight = 300; }
	
	if(parseInt(height) > 300)
	{
		if(height > docHeight)
		{
			$('#pasteFrame').height(docHeight);
		}
		else
		{
			$('#pasteFrame').height(height);
		}
	}
	else
	{
		$('#pasteFrame').height("300px");
	}
}

var num = false;
function lineNumbers()
{
	var src = $('#pasteFrame').attr('src');
	if(num)
	{
		src = src.replace("linenum=true", "linenum=false");
	}
	else
	{
		src = src.replace("linenum=false", "linenum=true");
	}
	$('#pasteFrame').attr("src", src);
	num = !num;
	
}

$(document).ready(function()
{
	$('#historyButton').tinytip("<div class='font11 center'>See how this paste evolved</div>", "bottomMiddle", "topMiddle");
	$('#new_version').tinytip("<div class='font11 center'>Create a copy of this paste that you can edit</div>", "bottomMiddle", "topMiddle");
	$('#save_paste').tinytip("<div class='font11 center'>Save this paste to your computer</div>", "bottomMiddle", "topMiddle");
	$('#show_full').tinytip("<div class='font11 center'>View this paste in full screen mode</div>", "bottomMiddle", "topMiddle");
	$('#view_stats').tinytip("<div class='font11 center'>View traffic statistics about this paste</div>", "bottomMiddle", "topMiddle");
	$('#share_paste').tinytip("<div class='font11 center'>Share this paste with your friends</div>", "bottomMiddle", "topMiddle");
	$('#lineNumbers').tinytip("<div class='font11 center'>Turn Line Numbers Off/On</div>", "bottomMiddle", "topMiddle");
	$('#embed_code').tinytip("<div class='font11 center'>Embed this paste on your site</div>", "bottomMiddle", "topMiddle");

});