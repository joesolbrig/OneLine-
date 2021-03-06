<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
     xmlns:dc="http://purl.org/dc/elements/1.1/" version="1.0">
<xsl:template match="//rss">
  <html>
  <body>
    <h1><xsl:value-of select="/rss/channel/title"/></h1>
    <p><xsl:value-of select="/rss/channel/description"/></p>
    <xsl:element name="a">
      	<xsl:attribute name="href">
      		<xsl:value-of select="link"/>
      	</xsl:attribute>
      	Read more...
    </xsl:element>
    <xsl:for-each select="/rss/channel/item">
        <h2><xsl:value-of select="title"/></h2>
        <p><xsl:value-of select="description"/></p>
        <p>
    <xsl:element name="a">
      	<xsl:attribute name="href">
        	<xsl:value-of select="link"/>
      	</xsl:attribute>
      	Read more...
    </xsl:element>
    </p>
    <p style="color: #ccc; font-size: 0.7em;"><xsl:value-of select="dc:creator"/></p>
    <p style="color: #ccc; font-style: italic; font-size: 0.7em;"><xsl:value-of select="dc:date"/></p>
    </xsl:for-each>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>
