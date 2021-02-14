#!/usr/bin/env python3

""" Multilocator visualizer tool """

import argparse
import json
import os
import re
import random
import sys
import time

from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import pyqtgraph as pg
import numpy as np

from PIL import Image
import paho.mqtt.client as mqtt

WINDOW_WIDTH = 1024 # * 2
WINDOW_HEIGHT = 768 # * 2

DEBUG_MODE = 0
MAX_NUM_TAGS = 300

PLOT_VIEW_3D = 1            # if 0, uses 2D view
PLOT_TAG_ADDRESSES = 1      # show tag address next to marker

PLOT_ROOM = 0               # only available in 3D view
PLOT_DEBUG_LINES = 1        # only available in 3D view
PLOT_DEBUG_LOCATORS = 1     # only available in 3D view
PLOT_MARKER_TRACES = 0      # only available in 3D view

MAX_NUM_TAG_LINES = 2       # if PLOT_DEBUG_LINES == 1, show lines to this many tags 
MAX_NUM_LOCATOR_LINES = 5   # if PLOT_DEBUG_LINES == 1, show lines from this many locators

DEFAULT_CONFIG = os.path.join(os.path.dirname(__file__), "../aoa_multilocator/config/multilocator_config.json")
DEFAULT_CONNECTION = {"host": "localhost", "port": 1883}

pg.setConfigOptions(enableExperimental=True, useOpenGL=True)

class TextGLViewWidget(gl.GLViewWidget):
  def __init__(self):
    super(TextGLViewWidget, self).__init__()
    self.tagPositions = {} # Key shall be the Bluetooth address and value shall be [x, y, z]

  def paintGL(self, *args, **kwds):
    gl.GLViewWidget.paintGL(self, *args, **kwds)

    if PLOT_VIEW_3D: 
      for key, value in self.tagPositions.items():
        try:
          self.renderText(value[0], value[1], value[2], key)
        except:
          pass

  def setText(self, addr, pos):
    self.tagPositions[addr] = pos

class Visualizer(object):
  def __init__(self):
    self.marker_trace = False
    if PLOT_MARKER_TRACES:
      self.marker_trace = True

    self.markerTraces = []
    self.markerTraceStep = 10
    self.numMarkerTraces = 20
    self.numEstimatesPlotted = 0
    self.update_all = 0
    self.plotlines = PLOT_DEBUG_LINES
    self.tagColors = {}

    self.numLocators = 0
    self.lineUpdateCount = [0] * MAX_NUM_TAGS
    self.azimuths = [{} for _ in range(MAX_NUM_TAG_LINES)]
    self.elevations = [{} for _ in range(MAX_NUM_TAG_LINES)]
    self.distances = [{} for _ in range(MAX_NUM_TAG_LINES)]
    self.locPositionsX = {}
    self.locPositionsY = {}
    self.locPositionsZ = {}
    self.locOrientationsX = {}
    self.locOrientationsY = {}
    self.locOrientationsZ = {}

    self.app = QtGui.QApplication(sys.argv)

    self.w = None
    self.view = None
    self.tagTexts = {}

    if PLOT_VIEW_3D:
      self.w = TextGLViewWidget() 
      self.w.opts['distance'] = 25
      self.w.setGeometry(200, 200, WINDOW_WIDTH, WINDOW_HEIGHT)
      self.w.orbit(225, 90)
      self.w.show()

      planeColor = [226.0 / 255.0, 205.0 / 255.0, 155.0 / 255.0, 0.5]
      z = np.zeros((20, 20))
      p1 = gl.GLSurfacePlotItem(z=z, shader='shaded', color=planeColor, glOptions='additive')
      p1.translate(-10, -10, 0)
      self.w.addItem(p1)

      xgrid = gl.GLGridItem(glOptions='additive')
      ygrid = gl.GLGridItem(glOptions='additive')
      zgrid = gl.GLGridItem(glOptions='additive')

#      self.w.addItem(xgrid)
#      self.w.addItem(ygrid)
      self.w.addItem(zgrid)

      xgrid.rotate(90, 0, 1, 0)
      ygrid.rotate(90, 1, 0, 0)

    else:
      # PLOT_VIEW_2D
      self.view = pg.PlotWidget()
      self.view.showGrid(x=True, y=True)
      self.view.resize(WINDOW_WIDTH, WINDOW_HEIGHT)
      self.view.show()
      self.view.setXRange(-15, 15)
      self.view.setYRange(-15, 15)
    
    if PLOT_DEBUG_LOCATORS:
      im = Image.open(r"img/locator.png")
      tex = np.array(im)
      scale = 2000.0

      imB = Image.open(r"img/locator_back.png")
      texB = np.array(imB)

      self.locators = []
      self.locatorsB = []
      self.locatorsInitialized = False

      for i in range(10):
        loc = gl.GLImageItem(tex, glOptions='translucent')
        loc.translate(-tex.shape[0]/(2.0 * scale), -tex.shape[1]/(2.0 * scale), 0.02)
        loc.scale(1.0/scale, 1.0/scale, 1.0)
        self.locators.append(loc)

        locB = gl.GLImageItem(texB, glOptions='translucent')
        locB.translate(-texB.shape[0]/(2.0 * scale), -texB.shape[1]/(2.0 * scale), -0.02)
        locB.scale(1.0/scale, 1.0/scale, 1.0)
        self.locatorsB.append(locB)

    self.markers = dict()
    self.tags = dict()
    self.multilocator_id = None

    if PLOT_VIEW_3D:
      axis = gl.GLAxisItem()
      axis.setSize(x=15, y=15, z=15)
      self.w.addItem(axis)

    for tagID in range(MAX_NUM_TAGS):
      self.tagColors[tagID] = [random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1), 1.0]

    if PLOT_VIEW_3D:
      if self.plotlines:
        self.lines = []
        # Line plots are drawn for max 4 locators only (performance restriction)
        for locator in range(MAX_NUM_LOCATOR_LINES):
          self.lines.append([])

          for tag in range(MAX_NUM_TAG_LINES):
            self.lines[locator].append(gl.GLLinePlotItem(pos=np.asarray([[0.0, 0.0, 0.0], [0.0, 0.0, 0.0]]), color=self.tagColors[tag]))
            self.w.addItem(self.lines[locator][tag])
        
  def start(self):
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()

  def plot_room(self):
    # Only supported in 3D mode

    if PLOT_VIEW_3D:
      # This is an example on how to plot simple objects into the 3D view
      vertexes = np.array([[1, 0, 0], #0
                       [0, 0, 0], #1
                       [0, 1, 0], #2
                       [0, 0, 1], #3
                       [1, 1, 0], #4
                       [1, 1, 1], #5
                       [0, 1, 1], #6
                       [1, 0, 1]], dtype=int)#7

      faces = np.array([[1,0,7], [1,3,7],
                    [1,2,4], [1,0,4],
                    [1,2,6], [1,3,6],
                    [0,4,5], [0,7,5],
                    [2,4,5], [2,6,5],
                    [3,6,5], [3,7,5]])

      shelfColor = [226.0 / 255.0, 205.0 / 255.0, 155.0 / 255.0, 0.5]
      tableColor = [100.0 / 255.0, 100.0 / 255.0, 100.0 / 255.0, 0.5]

      md = gl.MeshData(vertexes=vertexes, faces=faces)

      # shelf
      self.m1 = gl.GLMeshItem(meshdata=md, smooth=True, color=shelfColor, glOptions='translucent') 
      self.m1.translate(1.25, -5.9, 0.0)
      self.m1.scale(3.20, 0.57, 1.26)
      self.w.addItem(self.m1) 

      # table
      self.m16 = gl.GLMeshItem(meshdata=md, smooth=True, color=tableColor, glOptions='translucent')
      self.m16.translate(1.27, -4.5, 0.7)
      self.m16.scale(2.0, 0.8, 0.05)
      self.w.addItem(self.m16) 

  def set_locdata(self):
    if PLOT_VIEW_3D != 1:
      return
    i = 0
    for key in self.locPositionsX:
      self.locators[i].rotate(self.locOrientationsZ[key], x=0, y=0, z=1)
      self.locators[i].rotate(self.locOrientationsY[key], x=0, y=1, z=0)
      self.locators[i].rotate(self.locOrientationsX[key], x=1, y=0, z=0)
      self.locators[i].translate(self.locPositionsX[key], self.locPositionsY[key], self.locPositionsZ[key])
      self.w.addItem(self.locators[i])

      self.locatorsB[i].rotate(self.locOrientationsZ[key], x=0, y=0, z=1)
      self.locatorsB[i].rotate(self.locOrientationsY[key], x=0, y=1, z=0)
      self.locatorsB[i].rotate(self.locOrientationsX[key], x=1, y=0, z=0)
      self.locatorsB[i].translate(self.locPositionsX[key], self.locPositionsY[key], self.locPositionsZ[key])
      self.w.addItem(self.locatorsB[i])
      i += 1

  def set_linedata(self, tag):
    if tag >= MAX_NUM_TAG_LINES:
      return

    for loc in range(min(self.numLocators, MAX_NUM_LOCATOR_LINES)):
      loc_x = self.locPositionsX[loc]
      loc_y = self.locPositionsY[loc]
      loc_z = self.locPositionsZ[loc]

      azimuth = self.azimuths[tag][loc]
      elevation = self.elevations[tag][loc]
      distance = self.distances[tag][loc]

      start = [loc_x, loc_y, loc_z]
      end_x = loc_x + distance * np.cos(azimuth * np.pi / 180.0) * np.cos(elevation * np.pi / 180.0) 
      end_y = loc_y + distance * np.sin(azimuth * np.pi / 180.0) * np.cos(elevation * np.pi / 180.0) 
      end_z = loc_z + distance * np.sin(elevation * np.pi / 180.0) 
      end = [end_x, end_y, end_z]

      self.lines[loc][tag].setData(pos=np.asarray([start, end]))

  def set_plotdata(self, points, tagID):

    if tagID in self.markers:
      self.markers[tagID].setData(pos=np.array(points))


      #self.markers[tagID].setDepthValue(0.5)
    else:
      if PLOT_VIEW_3D:
        self.markers[tagID] = gl.GLScatterPlotItem(pos=np.array(points), color=self.tagColors[tagID], size=40)
        self.markers[tagID].setGLOptions('translucent')
        self.w.addItem(self.markers[tagID])

      else:
        # PLOT_VIEW_2D
        colo = self.tagColors[tagID]
        brushColor = QtGui.QColor(colo[0] * 255, colo[1] * 255, colo[2] * 255)

        self.markers[tagID] = pg.ScatterPlotItem(pen=pg.mkPen(width=1), brush=brushColor, symbol='o', size=40)
        self.markers[tagID].setData(pos=np.array(points))
        self.view.addItem(self.markers[tagID])

    # Plot a trailing marker trace
    if PLOT_VIEW_3D:
      if self.marker_trace and self.numEstimatesPlotted % self.markerTraceStep == 0:
        trace = gl.GLScatterPlotItem(pos=np.array(points), color=[0.6, 0.3, 0.8, 1.0], size=10)
        trace.setGLOptions('opaque')
        trace.setDepthValue(-0.5)
        self.markerTraces.insert(0, trace)
        self.w.addItem(self.markerTraces[0])

        if len(self.markerTraces) >= self.numMarkerTraces:
          item = self.markerTraces.pop()
          self.w.removeItem(item)
    
    self.numEstimatesPlotted += 1

  def setText(self, tagID, addr, points):
    if PLOT_VIEW_3D:
      self.w.setText(addr, points)

    else:
      # PLOT_VIEW_2D
      if tagID in self.tagTexts:
        self.tagTexts[tagID].setPos(points[0], points[1])
      else:
        self.tagTexts[tagID] = pg.TextItem(addr, color=(200, 200, 200), fill=None)
        self.tagTexts[tagID].setPos(points[0], points[1])
        self.view.addItem(self.tagTexts[tagID])

  def append_data(self, data_array, data):
    data_array.insert(0, data)

    if len(data_array) >= self.debug_data_size:
      data_array.pop()

  def add_items(self):
    for key, value in self.markers:
      self.w.addItem(value)
#    self.w.addItem(self.locators)

  def handle_kalman_data(self, debug_data):
    x = debug_data[0]
    y = debug_data[1]
    z = debug_data[2]
    vx = debug_data[3]
    vy = debug_data[4]
    vz = debug_data[5] 
    ax = debug_data[6]
    ay = debug_data[7]
    az = debug_data[8]

    ori = debug_data[11]
    angvel = debug_data[12]

    self.append_data(self.accx_data, ax)
    self.append_data(self.accy_data, ay)
    self.append_data(self.accz_data, az)

    self.append_data(self.velx_data, vx)
    self.append_data(self.vely_data, vy)
    self.append_data(self.velz_data, vz)

    self.append_data(self.posx_data, x)
    self.append_data(self.posy_data, y)
    self.append_data(self.posz_data, z)

    self.append_data(self.orix_data, ori[0])
    self.append_data(self.oriy_data, ori[1])
    self.append_data(self.oriz_data, ori[2])

    self.append_data(self.angvelx_data, angvel[0])
    self.append_data(self.angvely_data, angvel[1])
    self.append_data(self.angvelz_data, angvel[2])

  def handle_imu_data(self, debug_data):
    acc = debug_data[9]
    gyro = debug_data[10]
    ori = debug_data[13]

    self.append_data(self.imu_accx_data, acc[0])
    self.append_data(self.imu_accy_data, acc[1])
    self.append_data(self.imu_accz_data, acc[2])

    self.append_data(self.gyrox_data, gyro[0])
    self.append_data(self.gyroy_data, gyro[1])
    self.append_data(self.gyroz_data, gyro[2])

    self.append_data(self.imu_orix_data, ori[0])
    self.append_data(self.imu_oriy_data, ori[1])
    self.append_data(self.imu_oriz_data, ori[2])

  def debugUpdate(self, debug_data, update_imu):
    self.handle_kalman_data(debug_data) 

    if update_imu:
      self.handle_imu_data(debug_data) 

    # Acceleration update
    # Kalman
    self.accx_curve.setData(self.accx_data)
    self.accy_curve.setData(self.accy_data)
    self.accz_curve.setData(self.accz_data)

    # IMU
    self.imu_accx_curve.setData(self.imu_accx_data)
    self.imu_accy_curve.setData(self.imu_accy_data)
    self.imu_accz_curve.setData(self.imu_accz_data)

    # Velocity update
    # Kalman
    self.velx_curve.setData(self.velx_data)
    self.vely_curve.setData(self.vely_data)
    self.velz_curve.setData(self.velz_data)

    # IMU
    self.imu_velx_curve.setData(self.imu_velx_data)
    self.imu_vely_curve.setData(self.imu_vely_data)
    self.imu_velz_curve.setData(self.imu_velz_data)

    # Position update 
    # Kalman
    self.posx_curve.setData(self.posx_data)
    self.posy_curve.setData(self.posy_data)
    self.posz_curve.setData(self.posz_data)

    # IMU
    self.imu_posx_curve.setData(self.imu_posx_data)
    self.imu_posy_curve.setData(self.imu_posy_data)
    self.imu_posz_curve.setData(self.imu_posz_data)

    # Gyro Update
    # Gyro
    self.gyrox_curve.setData(self.gyrox_data)
    self.gyroy_curve.setData(self.gyroy_data)
    self.gyroz_curve.setData(self.gyroz_data)
   
    # Kalman 
    self.angvelx_curve.setData(self.angvelx_data)
    self.angvely_curve.setData(self.angvely_data)
    self.angvelz_curve.setData(self.angvelz_data)

    # Orientation Update
    # Kalman
    self.orix_curve.setData(self.orix_data)
    self.oriy_curve.setData(self.oriy_data)
    self.oriz_curve.setData(self.oriz_data)

    # IMU
    self.imu_orix_curve.setData(self.imu_orix_data)
    self.imu_oriy_curve.setData(self.imu_oriy_data)
    self.imu_oriz_curve.setData(self.imu_oriz_data)

  def update(self):
    for tag_id in self.tags:
      if self.tags[tag_id]["processed"] == False:
        self.tags[tag_id]["processed"] = True
        coordinates = [self.tags[tag_id]["position"]["x"], self.tags[tag_id]["position"]["y"], self.tags[tag_id]["position"]["z"]]
        self.set_plotdata(points=[coordinates], tagID=self.tags[tag_id]["sequence_nr"])
        if PLOT_TAG_ADDRESSES:
          self.setText(tag_id, tag_id, coordinates)


  def animation(self):
    timer = QtCore.QTimer()
    timer.timeout.connect(self.update)
    timer.setInterval(1)
    timer.start()
    self.start()

  def parse_config(self, conf_file):
    locators = []
    with open(conf_file, "rb") as conf:
        config = json.load(conf)
        locators = config["locators"]
        self.multilocator_id = config["id"]
    self.numLocators = len(locators)

    for loc in locators:
        key = loc["id"]
        self.locPositionsX[key] = loc["coordinate"]["x"]
        self.locPositionsY[key] = loc["coordinate"]["y"]
        self.locPositionsZ[key] = loc["coordinate"]["z"]
        self.locOrientationsX[key] = loc["orientation"]["x"]
        self.locOrientationsY[key] = loc["orientation"]["y"]
        self.locOrientationsZ[key] = loc["orientation"]["z"]

    self.locatorsInitialized = True
    self.set_locdata()

def mqtt_conn_type(arg):
    """ Argument parser for MQTT server connection parameters. """
    retval = DEFAULT_CONNECTION
    arglist = arg.split(":", 1)
    if len(arglist[0]) == 0:
        raise argparse.ArgumentTypeError("Host name is empty")
    retval["host"] = arglist[0]
    if len(arglist) > 1:
        try:
            retval["port"] = int(arglist[1])
        except ValueError as val:
            raise argparse.ArgumentTypeError("Invalid port number: " + arglist[1]) from val
    return retval

def on_connect(client, userdata, flags, rc):
    ''' Called when a CONNACK response is received from the server. '''
    print("Connected with result code " + str(rc))
    topic = "silabs/aoa/position/{}/#".format(userdata.multilocator_id)
    print("Subscribe for ", topic)
    client.subscribe(topic)

def on_message(client, userdata, msg):
    ''' Called when a PUBLISH message is received from the server. '''
    m = re.match(r"silabs/aoa/position/.+/(?P<tag_id>.+)", msg.topic)
    if m is not None:
      tag_id = m.group("tag_id")
      if tag_id not in userdata.tags:
        userdata.tags[tag_id] = dict()
        userdata.tags[tag_id]["sequence_nr"] = len(userdata.tags)
      userdata.tags[tag_id]["position"] = json.loads(msg.payload)
      userdata.tags[tag_id]["processed"] = False

def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-c", metavar="CONFIG_FILE", help="Configuration file path", default=DEFAULT_CONFIG)
    parser.add_argument("-m", metavar="HOST[:PORT]", help="MQTT broker connection parameters", default=DEFAULT_CONNECTION, type=mqtt_conn_type)
    args = parser.parse_args()

    v = Visualizer()
    v.parse_config(args.c)

    client = mqtt.Client(userdata=v)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(host=args.m["host"], port=args.m["port"])
    client.loop_start()

    if PLOT_ROOM:
      v.plot_room()
    v.add_items()
    v.animation()

if __name__ == "__main__":
  main()

