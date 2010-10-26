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
    
    Drupal.uwpeople = (function () {
      var _dest = Drupal.settings.uwpeople.destination_path;
      var _redirTimeout = Drupal.settings.uwpeople.redirect_timeout;
      var _ajaxURL = Drupal.settings.uwpeople.ajax_url;
      var _token = null;
      
      function dstatus(msg) {
        $("#message").append('<div class="message status">' + msg + '</div>');
      }
      
      function derror(msg) {
        $("#message").append('<div class="message error">' + msg + '</div>');
      }
      
      return {
        dcontext : context,
        
        canStart : function () {
          return Drupal.settings.uwpeople.ajax_token ? true : false;
        },
        
        start : function () {
          _token = Drupal.settings.uwpeople.ajax_token;
          
          $.ajax({
            type : "POST",
            url : _ajaxURL,
            dataType : "json",
            data : { action : "start", token : _token }
          });
        },
        
        getProgress : function (callback) {
          $.ajax({
            type : "POST",
            url : _ajaxURL,
            dataType : "json",
            data : { action : "progress", token : _token },
            success : function (data) {
              var ret = { error : 0, progress : 0 };
              
              if (data.error) {
                derror("Server error fetching progress - check log for details.");
                ret.error = true;
              }
              else {
                if (typeof(data.progress) !== "undefined" &&
                    typeof(data.message) !== "undefined") {
                  ret.error = false;
                  ret.progress = data.progress;
                  ret.message = data.message;
                }
              }
              
              callback(ret);
            }
          });
        },
        
        cancel : function () {
          window.location = _dest;
        },
        
        redirect : function() {
          setTimeout(function () { window.location = _dest; }, _redirTimeout);
        },
        
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
    
    $("input#uwpeople-refresh-cancel").click(function (event) {
      event.preventDefault();
      Drupal.uwpeople.cancel();
    });
    
    var _progress = new Drupal.progressBar("uwpeople_progress");
    $("#uwpeople_progress_container").append(_progress.element);
    
    if (Drupal.uwpeople.canStart()) {
      var _refreshTimeout = Drupal.settings.uwpeople.refresh_timeout;
      
      _progress.setProgress(-1, Drupal.t("Starting..."));
      Drupal.uwpeople.start();
      
      var refresh = function () {
        Drupal.uwpeople.getProgress(function (prog) {
          if (prog.error) {
            Drupal.uwpeople.finish();
            return;
          }
          
          _progress.setProgress(prog.progress, Drupal.t(prog.message));
          
          if (prog.progress >= 100)
            Drupal.uwpeople.finish();
          else
            setTimeout(function() { refresh(); }, _refreshTimeout);
        });
      };
      
      setTimeout(function() { refresh(); }, _refreshTimeout);
    }
    else {
      Drupal.uwpeople.redirect();
    }
  }
})(jQuery);
