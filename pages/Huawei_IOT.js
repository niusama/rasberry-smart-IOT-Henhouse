// pages/Huawei_IOT.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    result:'等待获取token',
  },
   /**
     * 获取token按钮按下：
     */
    gettoken:function(){
      console.log("开始获取。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      wx.request({
          url: 'https://iam.cn-north-4.myhuaweicloud.com/v3/auth/tokens',
          data:'{"auth":{"identity":{"methods":["password"],"password":{"user":{"name":"niusama","password":"13685081308bin","domain":{"name":"hw055758848"}}}},"scope":{"project":{"name":"cn-north-4"}}}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json' }, // 请求的 header 
          success: function(res){// success
            // success
              console.log("获取token成功");//打印完整消息
              console.log(res);//打印完整消息
            
            var token='';
            token=JSON.stringify(res.header['X-Subject-Token']);//解析消息头token
            token=token.replaceAll("\"", "");
            console.log("获取token=\n"+token);//打印token
            wx.setStorageSync('token',token);//把token写到缓存中,以便可以随时随地调用

          },
          fail:function(){
              // fail
              console.log("获取token失败");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("获取token完成");//打印完整消息
          } 
      });
  },
    touchBtn_gettoken:function()
    {
        console.log("获取token按钮按下");
        this.gettoken();

    },
    /**
     * 获取设备影子按钮按下：
     */

     getshadow:function () {
      console.log("开始获取影子");//打印完整消息
      var that=this; //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      console.log("我的toekn:"+token);//打印完整消息
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/shadow',
          data:' ',
          method: 'GET', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token 
        }, //请求的header 
          success: function(res){// success
            // success
              console.log(res);//打印完整消息
            var shadow=JSON.stringify(res.data.shadow[0].reported.properties);
            console.log('设备影子数据：'+shadow);
            var Temp=JSON.stringify(res.data.shadow[0].reported.properties.DHT11_T);
            var Humi=JSON.stringify(res.data.shadow[0].reported.properties.DHT11_H);
            var BEEP=JSON.stringify(res.data.shadow[0].reported.properties.BEEP);
            var MQ2=JSON.stringify(res.data.shadow[0].reported.properties.MQ2);
            var Water=JSON.stringify(res.data.shadow[0].reported.properties.Water);
            var Flame=JSON.stringify(res.data.shadow[0].reported.properties.Flame);
            var Light=JSON.stringify(res.data.shadow[0].reported.properties.Light);
            var LED1=JSON.stringify(res.data.shadow[0].reported.properties.LED1);
            var LED2=JSON.stringify(res.data.shadow[0].reported.properties.LED2);
            var LED3=JSON.stringify(res.data.shadow[0].reported.properties.LED3);
            console.log('温度='+Temp+'℃');
            console.log('湿度='+Humi+'%');
            that.setData({result:'温度:'+Temp+'℃,湿度:'+Humi+'%,'+'蜂鸣器:'+BEEP+',烟雾:'+MQ2+',雨滴:'+Water+'，火焰：'+Flame+'，光线：'+Light+'，灯光1：'+LED1+'灯光2：'+LED2+'灯光3:'+LED3});
          },
          fail:function(){
              // fail
              console.log("获取影子失败");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("获取影子完成");//打印完整消息
          } 
      });
     },
    touchBtn_getshadow:function()
    {
        console.log("获取设备影子按钮按下");
        this.getshadow();
    },
     /**
     * 设备命令下发按钮按下：
     */
        /**
     * 设备命令下发
     */
    setCommand:function(){
      console.log("开始下发命令。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/commands',
          data:'{"service_id": "stm32","command_name": "BEEP_CTL","paras": { "BEEP": "ON"}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token }, //请求的header 
          success: function(res){// success
              // success
              console.log("BEEP_ON下发命令成功");//打印完整消息
              console.log(res);//打印完整消息

          },
          fail:function(){
              // fail
              console.log("命令下发失败");//打印完整消息
              console.log("请先获取token");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("命令下发完成");//打印完整消息
              that.setData({result:'BEEP_ON下发命令成功'});
          } 
      });
  },
  BEEP_ON:function()
    {
        console.log("设备命令下发按钮按下");
        this.setCommand();
    },


    BEPOF:function(){
      console.log("开始下发命令。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/commands',
          data:'{"service_id": "stm32","command_name": "BEEP_CTL","paras": { "BEEP": "OFF"}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token }, //请求的header 
          success: function(res){// success
              // success
              console.log("BEEP_OFF下发命令成功");//打印完整消息
              console.log(res);//打印完整消息

          },
          fail:function(){
              // fail
              console.log("命令下发失败");//打印完整消息
              console.log("请先获取token");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("命令下发完成");//打印完整消息
              that.setData({result:'BEEP_OFF下发命令成功'});
          } 
      });
  },
  BEEP_OFF:function()
    {
        console.log("设备命令下发按钮按下");
        this.BEPOF();
    },

    RLY_ON:function(){
      console.log("开始下发命令。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/commands',
          data:'{"service_id": "stm32","command_name": "RELAYS_CTL","paras": { "RELAYS": "ON"}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token }, //请求的header 
          success: function(res){// success
              // success
              console.log("继电器_ON下发命令成功");//打印完整消息
              console.log(res);//打印完整消息

          },
          fail:function(){
              // fail
              console.log("命令下发失败");//打印完整消息
              console.log("请先获取token");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("命令下发完成");//打印完整消息
              that.setData({result:'继电器_ON下发命令成功'});
          } 
      });
  },
  RELAYS_ON:function()
    {
        console.log("设备命令下发按钮按下");
        this.RLY_ON();
    },

    RLY_OFF:function(){
      console.log("开始下发命令。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/commands',
          data:'{"service_id": "stm32","command_name": "RELAYS_CTL","paras": { "RELAYS": "OFF"}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token }, //请求的header 
          success: function(res){// success
              // success
              console.log("RELAYS_OFF下发命令成功");//打印完整消息
              console.log(res);//打印完整消息

          },
          fail:function(){
              // fail
              console.log("命令下发失败");//打印完整消息
              console.log("请先获取token");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("命令下发完成");//打印完整消息
              that.setData({result:'RELAYS_OFF下发命令成功'});
          } 
      });
  },
  RELAYS_OFF:function()
    {
        console.log("设备命令下发按钮按下");
        this.RLY_OFF();
    },


    LD1_ON:function(){
      console.log("开始下发命令。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/commands',
          data:'{"service_id": "stm32","command_name": "LED1_CTL","paras": { "LED1": "ON"}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token }, //请求的header 
          success: function(res){// success
              // success
              console.log("LED1_ON下发命令成功");//打印完整消息
              console.log(res);//打印完整消息

          },
          fail:function(){
              // fail
              console.log("命令下发失败");//打印完整消息
              console.log("请先获取token");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("命令下发完成");//打印完整消息
              that.setData({result:'LED1_ON下发命令成功'});
          } 
      });
  },
  LED1_ON:function()
    {
        console.log("设备命令下发按钮按下");
        this.LD1_ON();
    },

    LD1_OFF:function(){
      console.log("开始下发命令。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/commands',
          data:'{"service_id": "stm32","command_name": "LED1_CTL","paras": { "LED1": "OFF"}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token }, //请求的header 
          success: function(res){// success
              // success
              console.log("LED1_OFF下发命令成功");//打印完整消息
              console.log(res);//打印完整消息

          },
          fail:function(){
              // fail
              console.log("命令下发失败");//打印完整消息
              console.log("请先获取token");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("命令下发完成");//打印完整消息
              that.setData({result:'LED1_OFF下发命令成功'});
          } 
      });
  },
  LED1_OFF:function()
    {
        console.log("设备命令下发按钮按下");
        this.LD1_OFF();
    },

    LD2_ON:function(){
      console.log("开始下发命令。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/commands',
          data:'{"service_id": "stm32","command_name": "LED2_CTL","paras": { "LED2": "ON"}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token }, //请求的header 
          success: function(res){// success
              // success
              console.log("LED2_ON下发命令成功");//打印完整消息
              console.log(res);//打印完整消息

          },
          fail:function(){
              // fail
              console.log("命令下发失败");//打印完整消息
              console.log("请先获取token");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("命令下发完成");//打印完整消息
              that.setData({result:'LED2_ON下发命令成功'});
          } 
      });
  },
  LED2_ON:function()
    {
        console.log("设备命令下发按钮按下");
        this.LD2_ON();
    },

    LD2_OFF:function(){
      console.log("开始下发命令。。。");//打印完整消息
      var that=this;  //这个很重要，在下面的回调函数中由于异步问题不能有效修改变量，需要用that获取
      var token=wx.getStorageSync('token');//读缓存中保存的token
      wx.request({
          url: 'https://9cd91d8b6e.st1.iotda-app.cn-north-4.myhuaweicloud.com/v5/iot/3bae89a89ab546b5b8308124e83a6676/devices/67027ef4feb1bf7f791c8938_dev2/commands',
          data:'{"service_id": "stm32","command_name": "LED2_CTL","paras": { "LED2": "OFF"}}',
          method: 'POST', // OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT
          header: {'content-type': 'application/json','X-Auth-Token':token }, //请求的header 
          success: function(res){// success
              // success
              console.log("LED2_OFF下发命令成功");//打印完整消息
              console.log(res);//打印完整消息

          },
          fail:function(){
              // fail
              console.log("命令下发失败");//打印完整消息
              console.log("请先获取token");//打印完整消息
          },
          complete: function() {
              // complete
              console.log("命令下发完成");//打印完整消息
              that.setData({result:'LED2_OFF下发命令成功'});
          } 
      });
  },
  LED2_OFF:function()
    {
        console.log("设备命令下发按钮按下");
        this.LD2_OFF();
    },
  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {

  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload() {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom() {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage() {

  }
})