#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, ['--new'])

# zoom in central node
netedit.setZoom("100", "0", "200")

# Change to create edge mode
netedit.createEdgeMode()

# Create one way edge
netedit.leftClick(match, -30, 230)
netedit.leftClick(match, 430, 230)

# change to move mode
netedit.moveMode()

# Try to move to origin position
netedit.moveElement(match, 215, 232, -40, 232)

# Now move to top
netedit.moveElement(match, 215, 232, 215, 432)

# rebuild network
netedit.rebuildNetwork()

# Check undo and redo
netedit.undo(match, 3)
netedit.redo(match, 3)

# save newtork
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
