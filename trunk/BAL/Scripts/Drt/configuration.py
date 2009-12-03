import commands

class configurationDRT :
    def __init__(self) :
        self.svg = False
        self.wml = False
        self.http = False
        self.media = False
        self.geolocation = False
        self.threeDTransforms = False
        self.leak = False
        self.plugin = False
        self.storage = False
        self.verbose = False

    def setSupportSVG(self, value) :
        self.svg = value

    def setSupportWML(self, value) :
        self.wml = value

    def setSupportHTTP(self, value) :
        self.http = value

    def setSupportMedia(self, value) :
        self.media = value

    def setSupportGeolocation(self, value) :
        self.geolocation = value

    def setSupport3DTransforms(self, value) :
        self.ThreeDTransforms = value

    def setSupportLeak(self, value) :
        self.leak = value
    
    def setSupportPlugin(self, value) :
        self.plugin = value

    def setSupportStorage(self, value) :
        self.storage = value

    def setVerbose(self, value) :
        self.verbose = value

    def supportSVG(self) :
        return self.svg

    def supportWML(self) :
        return self.wml

    def supportHTTP(self) :
        return self.http

    def supportMedia(self) :
        return self.media

    def supportGeolocation(self) :
        return self.geolocation

    def support3DTransforms(self) :
        return self.threeDTransforms

    def supportLeak(self) :
        return self.leak

    def supportPlugin(self) :
        return self.plugin

    def supportStorage(self) :
        return self.storage

    def isVerbose(self) :
        return self.verbose

    def parseConfig(self,path) :
        print "todo parseConfig"
        CmakeFile = open(path,'r')
        l_svg, l_wml, l_geolocation, l_plugin, l_threeDTransforms, l_storage, l_debug = "", "", "", "", "", "", ""
        for line in CmakeFile:
            if "ENABLE_SVG" in line :
                l_svg = line.split("=")[1]
                l_svg = l_svg.split("\n")[0]
                if l_svg == 'ON':
                    self.svg = True                   
            if "ENABLE_WML" in line :
                l_wml = line.split("=")[1]
                l_wml = l_wml.split("\n")[0]
                if l_wml == 'ON':
                    self.wml = True 
            if "ENABLE_VIDEO" in line :
                l_media = line.split("=")[1]
                l_media = l_media.split("\n")[0]
                if l_media == 'ON':
                    self.media = True 
            if "ENABLE_GEOLOCATION" in line :
                l_geolocation = line.split("=")[1]
                l_geolocation = l_geolocation.split("\n")[0]
                if l_geolocation == 'ON':
                    self.geolocation = True       
            if "ENABLE_NPAPI" in line :
                l_plugin = line.split("=")[1]
                l_plugin = l_plugin.split("\n")[0]
                if l_plugin == 'ON':
                    self.plugin = True 
            if "ENABLE_3D_RENDERING" in line :
                l_threeDTransforms = line.split("=")[1]
                l_threeDTransforms = l_threeDTransforms.split("\n")[0]
                if l_threeDTransforms == 'ON':
                    self.threeDTransforms = True 
            if "ENABLE_DOM_STORAGE" in line :
                l_storage = line.split("=")[1]
                l_storage = l_storage.split("\n")[0] 
                if l_storage == 'ON':
                    self.storage = True           
            if "ENABLE_DEBUG" in line :
                l_debug = line.split("=")[1]
                l_debug = l_debug.split("\n")[0]  
                if l_debug == 'ON':
                    self.debug = True 
                    self.leak = True   

