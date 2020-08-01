const {getMonitorInfo: WgetMonitorInfo} = require("bindings")("display-info-wrapper");


function getMonitorInfo() {
    let returnObj = WgetMonitorInfo();
    return returnObj;
}

exports.getMonitorInfo = getMonitorInfo;