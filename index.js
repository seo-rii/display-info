const {getMonitorCount: getMonitorCount} = require("bindings")("display-info-wrapper");


console.log(getMonitorCount());