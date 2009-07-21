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

    def parseConfig(self) :
        print "todo parseConfig"

