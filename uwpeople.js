/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/
 
/**
 * @file
 * JavaScript for AJAX refresh page.
 */

(function ($) {
  Drupal.uwpeople = { dcontext : null };
  
  Drupal.behaviors.uwpeople = function (context) {
    if (Drupal.uwpeople.dcontext !== null)
      return;
    
    /**
     * Namespace.
     */
    Drupal.uwpeople = (function () {
      var _dest = Drupal.settings.uwpeople.destination_path;
      var _redirTimeout = Drupal.settings.uwpeople.redirect_timeout;
      var _ajaxURL = Drupal.settings.uwpeople.ajax_url;
      var _token = null;
      var _conn = null;
      
      /**
       * Show a status message.
       */
      function dstatus(msg) {
        $("#people_refresh_message").append('<div class="message status">' + msg + '</div>');
      }
      
      /**
       * Show an error message.
       */
      function derror(msg) {
        $("#people_refresh_message").append('<div class="message error">' + msg + '</div>');
      }
      
      return {
        dcontext : context,
        
        /**
         * Returns <code>true</code> if a refresh can begin; <code>false</code>
         * otherwise.
         */
        canStart : function () {
          return Drupal.settings.uwpeople.ajax_token ? true : false;
        },
        
        /**
         * Starts refresh.
         */
        start : function () {
          _token = Drupal.settings.uwpeople.ajax_token;
          
          _conn = $.ajax({
            type : "POST",
            url : _ajaxURL,
            dataType : "json",
            data : { action : "start", token : _token }
          });
        },
        
        /**
         * Gets the current progress.
         */
        getProgress : function (callback) {
          $.ajax({
            type : "POST",
            url : _ajaxURL,
            dataType : "json",
            data : { action : "progress", token : _token },
            success : function (data) {
              var ret = { error : true, progress : 0, message : null };
              
              if (!data.error)
                ret.error = false;
              
              if (typeof(data.progress) !== "undefined" &&
                  typeof(data.message) !== "undefined") {
                ret.progress = data.progress;
                ret.message = data.message;
              }
              
              if (ret.error && ret.message)
                derror(ret.message + "<br/>Canceling operation...");
              
              callback(ret);
            }
          });
        },
        
        /**
         * Redirects back to the settings page.
         */
        redirect : function() {
          setTimeout(function () { window.location = _dest; }, _redirTimeout);
        },
        
        /**
         * Handles the cancel button.
         */
        cancel : function () {
          if (Drupal.uwpeople.canStart()) {
            dstatus("Canceling...");
            _conn.abort();
            Drupal.uwpeople.redirect();
          }
          else {
            window.location = _dest;
          }
        },
        
        /**
         * Finishes the operation.
         */
        finish : function () {
          $.ajax({
            type : "POST",
            url : _ajaxURL,
            data : { action : "finish", token : _token }
          });
          
          dstatus("You will now be re-directed back to the UW People settings page.");
          Drupal.uwpeople.redirect();
        }
      };
    })();
    
    // setup cancel button
    $("input#uwpeople-refresh-cancel").click(function (event) {
      event.preventDefault();
      Drupal.uwpeople.cancel();
    });
    
    // build progress bar
    var _progress = new Drupal.progressBar("uwpeople_progress");
    $("#uwpeople_progress_container").append(_progress.element);
    
    // perform operation
    if (Drupal.uwpeople.canStart()) {
      var _refreshTimeout = Drupal.settings.uwpeople.refresh_timeout;
      
      _progress.setProgress(-1, Drupal.t("Starting..."));
      Drupal.uwpeople.start();
      
      // refresh function
      var refresh = function () {
        Drupal.uwpeople.getProgress(function (prog) {
          _progress.setProgress(prog.progress, Drupal.t(prog.message));
          
          if (prog.error)
            setTimeout(function() { Drupal.uwpeople.redirect(); }, 1000);
          else if (prog.progress >= 100)
            Drupal.uwpeople.finish();
          else
            setTimeout(function() { refresh(); }, _refreshTimeout);
        });
      };
      
      // start refreshing
      setTimeout(function() { refresh(); }, _refreshTimeout);
    }
    else {
      // can't start so redirect back
      Drupal.uwpeople.redirect();
    }
  }
})(jQuery);
