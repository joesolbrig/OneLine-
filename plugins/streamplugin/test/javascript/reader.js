var dbg = function(s) {
	if(typeof console !== 'undefined')
		console.log("Reader: " + s);
};

/*
 * Taken from:
 *
 * Readability. An Arc90 Lab Experiment. 
 * Website: http://lab.arc90.com/experiments/readability
 * Source:  http://code.google.com/p/arc90labs-readability
 *
 * Copyright (c) 2009 Arc90 Inc
 * Readability is licensed under the Apache License, Version 2.0.
 *
 * Hacked/Forked by Hans J. Solbrig, 2012 for the Runway project
 *
 * The reader is intended to run only in a custom webkit window to provide simplified site view
 * 
**/


readStyle='style-text';
readSize='size-text';
readMargin='margin';

reader_css=document.createElement('LINK');
reader_css.rel='stylesheet';
reader_css.href='reader.css';
reader_css.type='text/css';
reader_css.media='screen';
document.getElementsByTagName('head')[0].appendChild(reader_css);


var reader = {
	version:     '0.5.1',
	iframeLoads: 0,
	frameHack:   false, /**
	                     * The frame hack is to workaround a firefox bug where if you
						 * pull content out of a frame and stick it into the parent element, the scrollbar won't appear.
						 * So we fake a scrollbar in the wrapping div.
						**/
	bodyCache:  null,   /* Cache the body HTML in case we need to re-use it later */
	
	/**
	 * All of the regular expressions in use within reader.
	 * Defined up here so we don't instantiate them repeatedly in loops.
	 **/
	regexps: {
		unlikelyCandidatesRe:   /combx|comment|disqus|foot|header|menu|meta|nav|rss|shoutbox|sidebar|sponsor/i,
		okMaybeItsACandidateRe: /and|article|body|column|main/i,
		positiveRe:             /article|body|content|entry|hentry|page|pagination|post|text/i,
		negativeRe:             /combx|comment|contact|foot|footer|footnote|link|media|meta|promo|related|scroll|shoutbox|sponsor|tags|widget/i,
		divToPElementsRe:       /<(a|blockquote|dl|div|img|ol|p|pre|table|ul)/i,
		replaceBrsRe:           /(<br[^>]*>[ \n\r\t]*){2,}/gi,
		replaceFontsRe:         /<(\/?)font[^>]*>/gi,
		trimRe:                 /^\s+|\s+$/g,
		normalizeRe:            /\s{2,}/g,
		killBreaksRe:           /(<br\s*\/?>(\s|&nbsp;?)*){1,}/g,
		videoRe:                /http:\/\/(www\.)?(youtube|vimeo)\.com/i
	},

	/**
	 * Runs reader.
	 * 
	 * Workflow:
	 *  1. Prep the document by removing script tags, css, etc.
	 *  2. Build reader's DOM tree.
	 *  3. Grab the article content from the current dom tree.
	 *  4. Replace the current DOM tree with the new one.
	 *  5. Read peacefully.
	 *
	 * @return void
	 **/
	init: function(preserveUnlikelyCandidates) {
		preserveUnlikelyCandidates = (typeof preserveUnlikelyCandidates == 'undefined') ? false : preserveUnlikelyCandidates;

		if(document.body && !reader.bodyCache)
			reader.bodyCache = document.body.innerHTML;
		
		reader.prepDocument();
		
		/* Build reader's DOM tree */
		var overlay        = document.createElement("DIV");
		var innerDiv       = document.createElement("DIV");
		var articleTools   = reader.getArticleTools();
		var articleTitle   = reader.getArticleTitle();
		var articleContent = reader.grabArticle(preserveUnlikelyCandidates);
		var articleFooter  = reader.getArticleFooter();

		if(reader.getInnerText(articleContent, false) == "") {
			document.body.innerHTML = reader.bodyCache;
			/** If we attempted to strip unlikely candidates on the first run through, and we ended up with no content, that may mean we stripped out the actual content so we couldn't parse it. So re-run init while preserving unlikely candidates to have a better shot at getting our content out properly.
			**/
			if(!preserveUnlikelyCandidates) {
				return reader.init(true);				
			} 
			return false;
			/*So the final result is the orginal article on failure HJS*/
		}

		overlay.id = "readOverlay";
		innerDiv.id = "readInner";

		/* Apply user-selected styling */
		document.body.className = readStyle;
		overlay.className       = readStyle;
		innerDiv.className      = readMargin + " " + readSize;

		/* Glue the structure of our document together. */
		articleContent.appendChild( articleFooter  );
		      innerDiv.appendChild( articleTitle   );
		      innerDiv.appendChild( articleContent );
		       overlay.appendChild( articleTools   );
		       overlay.appendChild( innerDiv       );

		/* Clear the old HTML, insert the new content. */
		document.body.innerHTML = "";
		document.body.insertBefore(overlay, document.body.firstChild);

		if(reader.frameHack) {
			var readOverlay = document.getElementById('readOverlay');
			readOverlay.style.height = '100%';
			readOverlay.style.overflow = 'auto';
		}
	},

	/**
	 * Get the article tools Element that has buttons like print.
	 *
	 * @return void
	 **/
	getArticleTools: function () {
		var articleTools = document.createElement("DIV");

		articleTools.id        = "readTools";
		articleTools.innerHTML = "\
			<a href='#' onclick='return window.location.reload()' title='Reload original page' id='reload-page'>Reload Original Page</a>";

		return articleTools;
	},
	
	/**
	 * Get the article title as an H1. Currently just uses document.title,
	 * we might want to be smarter in the future.
	 *
	 * @return void
	 **/
	getArticleTitle: function () {
		var articleTitle = document.createElement("H1");
		articleTitle.innerHTML = document.title;
		
		return articleTitle;
	},

	/**
	 * Get the footer with the reader mark etc.
	 *
	 * @return void
	 **/
	getArticleFooter: function () {
		var articleFooter = document.createElement("DIV");
		articleFooter.id = "readFooter";
		articleFooter.innerHTML = " <div class='footer-right' ><span class='version'>Reader version " + reader.version + "</span>\
					</div> ";
		return articleFooter;
	},
	
	/**
	 * Prepare the HTML document for reader to scrape it.
	 * This includes things like stripping javascript, CSS, and handling terrible markup.
	 * 
	 * @return void
	 **/
	prepDocument: function () {
		/**
		 * In some cases a body element can't be found (if the HTML is totally hosed for example)
		 * so we create a new body node and append it to the document.
		 */
		if(document.body === null)
		{
			body = document.createElement("body");
			try {
				document.body = body;		
			}
			catch(e) {
				document.documentElement.appendChild(body);
			}
		}

		var frames = document.getElementsByTagName('frame');
		if(frames.length > 0)
		{
			var bestFrame = null;
			var bestFrameSize = 0;
			for(var frameIndex = 0; frameIndex < frames.length; frameIndex++)
			{
				var frameSize = frames[frameIndex].offsetWidth + frames[frameIndex].offsetHeight;
				var canAccessFrame = false;
				try {
					frames[frameIndex].contentWindow.document.body;
					canAccessFrame = true;
				} catch(e) {}
				
				if(canAccessFrame && frameSize > bestFrameSize)
				{
					bestFrame = frames[frameIndex];
					bestFrameSize = frameSize;
				}
			}

			if(bestFrame)
			{
				var newBody = document.createElement('body');
				newBody.innerHTML = bestFrame.contentWindow.document.body.innerHTML;
				newBody.style.overflow = 'scroll';
				document.body = newBody;
				
				var frameset = document.getElementsByTagName('frameset')[0];
				if(frameset)
					frameset.parentNode.removeChild(frameset);
					
				reader.frameHack = true;
			}
		}


		/* remove all scripts that are not reader */
		var scripts = document.getElementsByTagName('script');
		for(i = scripts.length-1; i >= 0; i--)
		{
			if(typeof(scripts[i].src) == "undefined" || scripts[i].src.indexOf('reader') == -1)
			{
				scripts[i].parentNode.removeChild(scripts[i]);			
			}
		}

		/* remove all stylesheets */
		for (var k=0;k < document.styleSheets.length; k++) {
			if (document.styleSheets[k].href != null && document.styleSheets[k].href.lastIndexOf("reader") == -1) {
				document.styleSheets[k].disabled = true;
			}
		}

		/* Remove all style tags in head (not doing this on IE) - TODO: Why not? */
		var styleTags = document.getElementsByTagName("style");
		for (var j=0;j < styleTags.length; j++)
			if (navigator.appName != "Microsoft Internet Explorer")
				styleTags[j].textContent = "";

		/* Turn all double br's into p's */
		/* Note, this is pretty costly as far as processing goes. Maybe optimize later. */
		document.body.innerHTML = document.body.innerHTML.replace(reader.regexps.replaceBrsRe, '</p><p>').replace(reader.regexps.replaceFontsRe, '<$1span>')
	},

	/**
	 * Prepare the article node for display. Clean out any inline styles,
	 * iframes, forms, strip extraneous <p> tags, etc.
	 *
	 * @param Element
	 * @return void
	 **/
	prepArticle: function (articleContent) {
		reader.cleanStyles(articleContent);
		reader.killBreaks(articleContent);

		/* Clean out junk from the article content */
		reader.clean(articleContent, "form");
		reader.clean(articleContent, "object");
		reader.clean(articleContent, "h1");
		/**
		 * If there is only one h2, they are probably using it
		 * as a header and not a subheader, so remove it since we already have a header.
		***/
		if(articleContent.getElementsByTagName('h2').length == 1)
			reader.clean(articleContent, "h2");
		reader.clean(articleContent, "iframe");

		reader.cleanHeaders(articleContent);

		/* Do these last as the previous stuff may have removed junk that will affect these */
		reader.cleanConditionally(articleContent, "table");
		reader.cleanConditionally(articleContent, "ul");
		reader.cleanConditionally(articleContent, "div");

		/* Remove extra paragraphs */
		var articleParagraphs = articleContent.getElementsByTagName('p');
		for(i = articleParagraphs.length-1; i >= 0; i--)
		{
			var imgCount    = articleParagraphs[i].getElementsByTagName('img').length;
			var embedCount  = articleParagraphs[i].getElementsByTagName('embed').length;
			var objectCount = articleParagraphs[i].getElementsByTagName('object').length;
			
			if(imgCount == 0 && embedCount == 0 && objectCount == 0 && reader.getInnerText(articleParagraphs[i], false) == '')
			{
				articleParagraphs[i].parentNode.removeChild(articleParagraphs[i]);
			}
		}

		try {
			articleContent.innerHTML = articleContent.innerHTML.replace(/<br[^>]*>\s*<p/gi, '<p');		
		}
		catch (e) {
			dbg("Cleaning innerHTML of breaks failed. This is an IE strict-block-elements bug. Ignoring.");
		}
	},
	
	/**
	 * Initialize a node with the reader object. Also checks the
	 * className/id for special names to add to its score.
	 *
	 * @param Element
	 * @return void
	**/
	initializeNode: function (node) {
		node.reader = {"contentScore": 0};			

		switch(node.tagName) {
			case 'DIV':
				node.reader.contentScore += 5;
				break;

			case 'PRE':
			case 'TD':
			case 'BLOCKQUOTE':
				node.reader.contentScore += 3;
				break;
				
			case 'ADDRESS':
			case 'OL':
			case 'UL':
			case 'DL':
			case 'DD':
			case 'DT':
			case 'LI':
			case 'FORM':
				node.reader.contentScore -= 3;
				break;

			case 'H1':
			case 'H2':
			case 'H3':
			case 'H4':
			case 'H5':
			case 'H6':
			case 'TH':
				node.reader.contentScore -= 5;
				break;
		}

		node.reader.contentScore += reader.getClassWeight(node);
	},
	
	/***
	 * grabArticle - Using a variety of metrics (content score, classname, element types), find the content that is
	 *               most likely to be the stuff a user wants to read. Then return it wrapped up in a div.
	 *
	 * @return Element
	**/
	grabArticle: function (preserveUnlikelyCandidates) {
		/**
		 * First, node prepping. Trash nodes that look cruddy (like ones with the class name "comment", etc), and turn divs
		 * into P tags where they have been used inappropriately (as in, where they contain no other block level elements.)
		 *
		 * Note: Assignment from index for performance. See http://www.peachpit.com/articles/article.aspx?p=31567&seqNum=5
		 * TODO: Shouldn't this be a reverse traversal?
		**/
		for(var nodeIndex = 0; (node = document.getElementsByTagName('*')[nodeIndex]); nodeIndex++)
		{
			/* Remove unlikely candidates */
			if (!preserveUnlikelyCandidates) {
				var unlikelyMatchString = node.className + node.id;
				if (unlikelyMatchString.search(reader.regexps.unlikelyCandidatesRe) !== -1 &&
				    unlikelyMatchString.search(reader.regexps.okMaybeItsACandidateRe) == -1 &&
					node.tagName !== "BODY")
				{
					dbg("Removing unlikely candidate - " + unlikelyMatchString);
					node.parentNode.removeChild(node);
					nodeIndex--;
					continue;
				}				
			}

			/* Turn all divs that don't have children block level elements into p's */
			if (node.tagName === "DIV") {
				if (node.innerHTML.search(reader.regexps.divToPElementsRe) === -1)	{
					dbg("Altering div to p");
					var newNode = document.createElement('p');
					try {
						newNode.innerHTML = node.innerHTML;				
						node.parentNode.replaceChild(newNode, node);
						nodeIndex--;
					}
					catch(e)
					{
						dbg("Could not alter div to p, probably an IE restriction, reverting back to div.")
					}
				}
				else
				{
					/* EXPERIMENTAL */
					for(var i = 0, il = node.childNodes.length; i < il; i++) {
						var childNode = node.childNodes[i];
						if(childNode.nodeType == Node.TEXT_NODE) {
							dbg("replacing text node with a p tag with the same content.");
							var p = document.createElement('p');
							p.innerHTML = childNode.nodeValue;
							p.style.display = 'inline';
							p.className = 'reader-styled';
							childNode.parentNode.replaceChild(p, childNode);
						}
					}
				}
			} 
		}

		/**
		 * Loop through all paragraphs, and assign a score to them based on how content-y they look.
		 * Then add their score to their parent node.
		 *
		 * A score is determined by things like number of commas, class names, etc. Maybe eventually link density.
		**/
		var allParagraphs = document.getElementsByTagName("p");
		var candidates    = [];

		for (var j=0; j	< allParagraphs.length; j++) {
			var parentNode      = allParagraphs[j].parentNode;
			var grandParentNode = parentNode.parentNode;
			var innerText       = reader.getInnerText(allParagraphs[j]);

			/* If this paragraph is less than 25 characters, don't even count it. */
			if(innerText.length < 25)
				continue;

			/* Initialize reader data for the parent. */
			if(typeof parentNode.reader == 'undefined')
			{
				reader.initializeNode(parentNode);
				candidates.push(parentNode);
			}

			/* Initialize reader data for the grandparent. */
			if(typeof grandParentNode.reader == 'undefined')
			{
				reader.initializeNode(grandParentNode);
				candidates.push(grandParentNode);
			}

			var contentScore = 0;

			/* Add a point for the paragraph itself as a base. */
			contentScore++;

			/* Add points for any commas within this paragraph */
			contentScore += innerText.split(',').length;
			
			/* For every 100 characters in this paragraph, add another point. Up to 3 points. */
			contentScore += Math.min(Math.floor(innerText.length / 100), 3);
			
			/* Add the score to the parent. The grandparent gets half. */
			parentNode.reader.contentScore += contentScore;
			grandParentNode.reader.contentScore += contentScore/2;
		}

		/**
		 * After we've calculated scores, loop through all of the possible candidate nodes we found
		 * and find the one with the highest score.
		**/
		var topCandidate = null;
		for(var i=0, il=candidates.length; i < il; i++)
		{
			/**
			 * Scale the final candidates score based on link density. Good content should have a
			 * relatively small link density (5% or less) and be mostly unaffected by this operation.
			**/
			candidates[i].reader.contentScore = candidates[i].reader.contentScore * (1-reader.getLinkDensity(candidates[i]));

			dbg('Candidate: ' + candidates[i] + " (" + candidates[i].className + ":" + candidates[i].id + ") with score " + candidates[i].reader.contentScore);

			if(!topCandidate || candidates[i].reader.contentScore > topCandidate.reader.contentScore)
				topCandidate = candidates[i];
		}

		/**
		 * If we still have no top candidate, just use the body as a last resort.
		 * We also have to copy the body node so it is something we can modify.
		 **/
		if (topCandidate == null || topCandidate.tagName == "BODY")
		{
			topCandidate = document.createElement("DIV");
			topCandidate.innerHTML = document.body.innerHTML;
			document.body.innerHTML = "";
			document.body.appendChild(topCandidate);
			reader.initializeNode(topCandidate);
		}


		/**
		 * Now that we have the top candidate, look through its siblings for content that might also be related.
		 * Things like preambles, content split by ads that we removed, etc.
		**/
		var articleContent        = document.createElement("DIV");
	        articleContent.id     = "reader-content";
		var siblingScoreThreshold = Math.max(10, topCandidate.reader.contentScore * 0.2);
		var siblingNodes          = topCandidate.parentNode.childNodes;
		for(var i=0, il=siblingNodes.length; i < il; i++)
		{
			var siblingNode = siblingNodes[i];
			var append      = false;

			dbg("Looking at sibling node: " + siblingNode + " (" + siblingNode.className + ":" + siblingNode.id + ")" + ((typeof siblingNode.reader != 'undefined') ? (" with score " + siblingNode.reader.contentScore) : ''));
			dbg("Sibling has score " + (siblingNode.reader ? siblingNode.reader.contentScore : 'Unknown'));

			if(siblingNode === topCandidate)
			{
				append = true;
			}
			
			if(typeof siblingNode.reader != 'undefined' && siblingNode.reader.contentScore >= siblingScoreThreshold)
			{
				append = true;
			}
			
			if(siblingNode.nodeName == "P") {
				var linkDensity = reader.getLinkDensity(siblingNode);
				var nodeContent = reader.getInnerText(siblingNode);
				var nodeLength  = nodeContent.length;
				
				if(nodeLength > 80 && linkDensity < 0.25)
				{
					append = true;
				}
				else if(nodeLength < 80 && linkDensity == 0 && nodeContent.search(/\.( |$)/) !== -1)
				{
					append = true;
				}
			}

			if(append)
			{
				dbg("Appending node: " + siblingNode)

				/* Append sibling and subtract from our list because it removes the node when you append to another node */
				articleContent.appendChild(siblingNode);
				i--;
				il--;
			}
		}				

		/**
		 * So we have all of the content that we need. Now we clean it up for presentation.
		**/
		reader.prepArticle(articleContent);
		
		return articleContent;
	},
	
	/**
	 * Get the inner text of a node - cross browser compatibly.
	 * This also strips out any excess whitespace to be found.
	 *
	 * @param Element
	 * @return string
	**/
	getInnerText: function (e, normalizeSpaces) {
		var textContent    = "";

		normalizeSpaces = (typeof normalizeSpaces == 'undefined') ? true : normalizeSpaces;

		if (navigator.appName == "Microsoft Internet Explorer")
			textContent = e.innerText.replace( reader.regexps.trimRe, "" );
		else
			textContent = e.textContent.replace( reader.regexps.trimRe, "" );

		if(normalizeSpaces)
			return textContent.replace( reader.regexps.normalizeRe, " ");
		else
			return textContent;
	},

	/**
	 * Get the number of times a string s appears in the node e.
	 *
	 * @param Element
	 * @param string - what to split on. Default is ","
	 * @return number (integer)
	**/
	getCharCount: function (e,s) {
	    s = s || ",";
		return reader.getInnerText(e).split(s).length;
	},

	/**
	 * Remove the style attribute on every e and under.
	 * TODO: Test if getElementsByTagName(*) is faster.
	 *
	 * @param Element
	 * @return void
	**/
	cleanStyles: function (e) {
	    e = e || document;
	    var cur = e.firstChild;

		if(!e)
			return;

		// Remove any root styles, if we're able.
		if(typeof e.removeAttribute == 'function' && e.className != 'reader-styled')
			e.removeAttribute('style');

	    // Go until there are no more child nodes
	    while ( cur != null ) {
			if ( cur.nodeType == 1 ) {
				// Remove style attribute(s) :
				if(cur.className != "reader-styled") {
					cur.removeAttribute("style");					
				}
				reader.cleanStyles( cur );
			}
			cur = cur.nextSibling;
		}			
	},
	
	/**
	 * Get the density of links as a percentage of the content
	 * This is the amount of text that is inside a link divided by the total text in the node.
	 * 
	 * @param Element
	 * @return number (float)
	**/
	getLinkDensity: function (e) {
		var links      = e.getElementsByTagName("a");
		var textLength = reader.getInnerText(e).length;
		var linkLength = 0;
		for(var i=0, il=links.length; i<il;i++)
		{
			linkLength += reader.getInnerText(links[i]).length;
		}		

		return linkLength / textLength;
	},
	
	/**
	 * Get an elements class/id weight. Uses regular expressions to tell if this 
	 * element looks good or bad.
	 *
	 * @param Element
	 * @return number (Integer)
	**/
	getClassWeight: function (e) {
		var weight = 0;

		/* Look for a special classname */
		if (e.className != "")
		{
			if(e.className.search(reader.regexps.negativeRe) !== -1)
				weight -= 25;

			if(e.className.search(reader.regexps.positiveRe) !== -1)
				weight += 25;				
		}

		/* Look for a special ID */
		if (typeof(e.id) == 'string' && e.id != "")
		{
			if(e.id.search(reader.regexps.negativeRe) !== -1)
				weight -= 25;

			if(e.id.search(reader.regexps.positiveRe) !== -1)
				weight += 25;				
		}

		return weight;
	},
	
	/**
	 * Remove extraneous break tags from a node.
	 *
	 * @param Element
	 * @return void
	 **/
	killBreaks: function (e) {
		try {
			e.innerHTML = e.innerHTML.replace(reader.regexps.killBreaksRe,'<br />');		
		}
		catch (e) {
			dbg("KillBreaks failed - this is an IE bug. Ignoring.");
		}
	},

	/**
	 * Clean a node of all elements of type "tag".
	 * (Unless it's a youtube/vimeo video. People love movies.)
	 *
	 * @param Element
	 * @param string tag to clean
	 * @return void
	 **/
	clean: function (e, tag) {
		var targetList = e.getElementsByTagName( tag );
		var isEmbed    = (tag == 'object' || tag == 'embed');

		for (var y=targetList.length-1; y >= 0; y--) {
			/* Allow youtube and vimeo videos through as people usually want to see those. */
			if(isEmbed && targetList[y].innerHTML.search(reader.regexps.videoRe) !== -1)
			{
				continue;
			}

			targetList[y].parentNode.removeChild(targetList[y]);
		}
	},
	
	/**
	 * Clean an element of all tags of type "tag" if they look fishy.
	 * "Fishy" is an algorithm based on content length, classnames, link density, number of images & embeds, etc.
	 *
	 * @return void
	 **/
	cleanConditionally: function (e, tag) {
		var tagsList      = e.getElementsByTagName(tag);
		var curTagsLength = tagsList.length;

		/**
		 * Gather counts for other typical elements embedded within.
		 * Traverse backwards so we can remove nodes at the same time without effecting the traversal.
		 *
		 * TODO: Consider taking into account original contentScore here.
		**/
		for (var i=curTagsLength-1; i >= 0; i--) {
			var weight = reader.getClassWeight(tagsList[i]);

			dbg("Cleaning Conditionally " + tagsList[i] + " (" + tagsList[i].className + ":" + tagsList[i].id + ")" + ((typeof tagsList[i].reader != 'undefined') ? (" with score " + tagsList[i].reader.contentScore) : ''));

			if(weight < 0)
			{
				tagsList[i].parentNode.removeChild(tagsList[i]);
			}
			else if ( reader.getCharCount(tagsList[i],',') < 10) {
				/**
				 * If there are not very many commas, and the number of
				 * non-paragraph elements is more than paragraphs or other ominous signs, remove the element.
				**/

				var p      = tagsList[i].getElementsByTagName("p").length;
				var img    = tagsList[i].getElementsByTagName("img").length;
				var li     = tagsList[i].getElementsByTagName("li").length-100;
				var input  = tagsList[i].getElementsByTagName("input").length;

				var embedCount = 0;
				var embeds     = tagsList[i].getElementsByTagName("embed");
				for(var ei=0,il=embeds.length; ei < il; ei++) {
					if (embeds[ei].src.search(reader.regexps.videoRe) == -1) {
					  embedCount++;	
					}
				}

				var linkDensity   = reader.getLinkDensity(tagsList[i]);
				var contentLength = reader.getInnerText(tagsList[i]).length;
				var toRemove      = false;

				if ( img > p ) {
				 	toRemove = true;
				} else if(li > p && tag != "ul" && tag != "ol") {
					toRemove = true;
				} else if( input > Math.floor(p/3) ) {
				 	toRemove = true; 
				} else if(contentLength < 25 && (img == 0 || img > 2) ) {
					toRemove = true;
				} else if(weight < 25 && linkDensity > .2) {
					toRemove = true;
				} else if(weight >= 25 && linkDensity > .5) {
					toRemove = true;
				} else if((embedCount == 1 && contentLength < 75) || embedCount > 1) {
					toRemove = true;
				}

				if(toRemove) {
					tagsList[i].parentNode.removeChild(tagsList[i]);
				}
			}
		}
	},

	/**
	 * Clean out spurious headers from an Element. Checks things like classnames and link density.
	 *
	 * @param Element
	 * @return void
	**/
	cleanHeaders: function (e) {
		for (var headerIndex = 1; headerIndex < 7; headerIndex++) {
			var headers = e.getElementsByTagName('h' + headerIndex);
			for (var i=headers.length-1; i >=0; i--) {
				if (reader.getClassWeight(headers[i]) < 0 || reader.getLinkDensity(headers[i]) > 0.33) {
					headers[i].parentNode.removeChild(headers[i]);
				}
			}
		}
	},
	
	htmlspecialchars: function (s) {
		if (typeof(s) == "string") {
			s = s.replace(/&/g, "&amp;");
			s = s.replace(/"/g, "&quot;");
			s = s.replace(/'/g, "&#039;");
			s = s.replace(/</g, "&lt;");
			s = s.replace(/>/g, "&gt;");
		}
	
		return s;
	}
};

reader.init();
