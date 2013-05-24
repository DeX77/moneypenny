<?xml version="1.0" encoding="utf8" standalone="yes"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!--
***************************************************************************
 *   Copyright (C) 2005-2006 by Konrad Wilhelm Kleine
 *   konrad@plothe-kleine.de
 *
 *   This file is used to 'render' xml input to html output.
 *   The input is a driver description xml file and the output a formatted
 *   html document that can be used in a QMessageBox. Only a small part
 *   of the input is rendered, which is the English comment.
 *
 *   PLEASE NOTICE: If not stated otherwise, whenever we speak of 'printer
 *                  drivers' or simply 'drivers', we mean the PPD files
 *                  and not the actual drivers.
 *
 *   STATUS: File has been reviewed and commented
 *
 *   $Id: driverdetails.xslt,v 1.2 2010-09-28 16:50:26 hbci Exp $
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software Foundation,
 *   Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************
 -->

  <!--Preserve-Space Elements-->
  <xsl:preserve-space elements="" />

  <!--Output Method-->
  <xsl:output method="html" encoding="utf8" indent="yes"
        media-type="text/html" />

  <!--Parameters-->
  <xsl:param name="commentTitle" select="'Comment'" />

  <!--Main Entry-point-->
    <xsl:template match="/">
    <xsl:apply-templates select="/driver/comments/en" />
  </xsl:template>

  <xsl:template match="/driver/comments/en">
    <h1><xsl:value-of select="$commentTitle" /></h1>
    <xsl:element name="a"><xsl:attribute name="href"><xsl:value-of select="/driver/url" /></xsl:attribute>
      <xsl:value-of select="/driver/url" />
    </xsl:element>
    <hr />
    <xsl:apply-templates />
  </xsl:template>

</xsl:stylesheet>
