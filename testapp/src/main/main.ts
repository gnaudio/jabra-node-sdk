import { app, BrowserWindow, ipcMain } from "electron";
import * as os from "os";

import { ConfigParamsCloud, JabraError } from '@gnaudio/jabra-node-sdk';
import * as process from 'process';
import { JabraApiServerFactory, JabraApiServer } from '@gnaudio/jabra-electron-renderer-helper';

import * as path from "path";
import { openHelpWindow } from "../common/ipc";

// Get our own version information directly from packages:
const testPackage = require ('../../package.json');
const nodeSdkPackage = require ( '@gnaudio/jabra-node-sdk/package.json');
const electronHelperPackage = require ('@gnaudio/jabra-electron-renderer-helper/package.json');
const osType = `${os.platform()} (${os.arch()})`;

let mainWindow: BrowserWindow | null = null;
let jabraServerFactory : JabraApiServerFactory | null = null;
let jabraServer: JabraApiServer | null = null;

// Get optional command line runtime options:
const actualCommandLineOptions: any = {};
process.argv.slice(2, process.argv.length).forEach( (arg) => {
    if (arg.slice(0,2) === '--') {
        const longArg = arg.split('=');
        const longArgFlag = longArg[0].slice(2,longArg[0].length);
        const longArgValue = longArg.length > 1 ? longArg[1] : true;
        actualCommandLineOptions[longArgFlag] = longArgValue;
    }
});

const nonJabraDeviceDectectionArg: boolean = (actualCommandLineOptions.nonJabraDeviceDectection || "false") === "true";
const cloudParamsArg: ConfigParamsCloud = {
  blockAllNetworkAccess: actualCommandLineOptions.blockAllNetworkAccess ? (actualCommandLineOptions.blockAllNetworkAccess === "true") : undefined,
  baseUrl_capabilities: actualCommandLineOptions.baseUrl_capabilities || undefined,
  baseUrl_fw: actualCommandLineOptions.baseUrl_fw || undefined,
  proxy: actualCommandLineOptions.proxy || undefined
};

console.log("Using arguments: nonJabraDeviceDectectionArg="+nonJabraDeviceDectectionArg+ ", cloudParamsArg=" + JSON.stringify(cloudParamsArg));

/**
 * Create electon window returning a promise that resolves when
 * the window is fully loaded and thus ready to receive events.
 */
function createAndLoadWindow(): Promise<BrowserWindow> {
  // Create the browser window.
  let window = new BrowserWindow({
    height: 1024,
    width: 1280,
    webPreferences: {
      // Disabled Node integration
      nodeIntegration: false,
      // In a sandbox
      sandbox: true,
      // Allow Ipc to/from sandbox
      contextIsolation: false,
      // No insecure code.
      webSecurity: true,
      // Preload script
      preload: path.join(__dirname, 'preload.js')
    }
  });

  // Version/type info are passed as query args to our renderer window:
  const args = `testAppVersion=${testPackage.version}
                &nodeSdkVersion=${nodeSdkPackage.version}
                &electronHelperVersion=${electronHelperPackage.version}
                &electronVersion=${process.versions.electron}
                &nodeVersion=${process.versions.node}
                &osType=${osType}`.replace(/[\n\r\t\s]/g, '');

  const htmlUrl = `file://${__dirname}/../renderer/index.html?${args}`

  // load the index.html of the app returning a promise that resolves when loaded.
  // Nb. the promise part is new for electron 5 - alternatively, we could wait for
  // 'did-finish-load' and convert that into a promise.

  const loadPromise = window.loadURL(htmlUrl);

  // Open the DevTools.
  // window.webContents.openDevTools();

  // Emitted when the window is closed.
  window.on("closed", () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;
    shutdown();
  });
 
  window.on('unresponsive', () => {
    console.log('window crashed');
  });


  window.webContents.on('did-fail-load', () => {
    console.log('window failed load');
  });

  return loadPromise.then(() => {      
    mainWindow = window;
    return window;
  });
}

function shutdown() {
  if (jabraServer) {
     jabraServer.shutdown().catch((err: Error) => {
      console.log("Error during server shutdown " + err);
     });

    jabraServer = null;
  }
  if (app) {
    app.quit();
  }
}

function setup() {
  // First setup the jabra server factory singleton when electron is initialized but BEFORE creating GUI.
  if (!jabraServerFactory) {
    jabraServerFactory = new JabraApiServerFactory(ipcMain);
  }

  createAndLoadWindow().then((fullyLoadedWindow) => {   
    ipcMain.on(openHelpWindow, (event: any) => {
      const helpWindow = new BrowserWindow({
        height: 900,
        width: 1024,
        title: 'NodeJS SDK API Reference',
        webPreferences: {
          // Disabled Node integration
          nodeIntegration: false,
          // In a sandbox
          sandbox: true,
          // Allow Ipc to/from sandbox
          contextIsolation: false,
          // No insecure code.
          webSecurity: true,
        }
      });

      const apiDoc = require.resolve('@gnaudio/jabra-node-sdk/dist/doc/index.html');
      helpWindow.loadFile(apiDoc);
    });

    // As window is now fully loaded we can create our api server for the client.
    jabraServerFactory!.create('A7tSsfD42VenLagL2mM6i2f0VafP/842cbuPCnC+uE8=', cloudParamsArg, nonJabraDeviceDectectionArg, fullyLoadedWindow).then( (result) => {
      jabraServer = result;
      console.log("JabraApiServer sucessfully created");

      // If the main process need to use the JabraApi, here is the 
      // reference to it:
      const jabraApi = result.getJabraApi();

      return jabraServer;
    }).catch( (err: JabraError) => {
      console.log("Error during jabra application/server setup " + err);
    });
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on("ready", async () => {
  // Gui setup
  setup();
});

// Quit when all windows are closed.
app.on("window-all-closed", () => {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== "darwin") {
    shutdown();
  }
});

app.on("activate", () => {
  // On OS X it"s common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    setup();
  }
});

process.on('uncaughtException', (errorObject) => {
  let error : string = "";
  if (errorObject instanceof Error) {
    error = 'UnhandledRejection ' + errorObject.name;
    return;
  } else {
    error = JSON.stringify(errorObject);
  }

  console.error(error);
});

process.on('unhandledRejection', (rejection) => {
  let error : string = "";
  if (rejection instanceof Error) {
    error = 'UnhandledRejection ' + rejection.name;
    return;
  } else {
    error = JSON.stringify(rejection);
  }

  console.error(error);
});
