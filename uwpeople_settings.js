/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/
 
/**
 * @file
 * JavaScript to be used on the UW People settings page.
 */

(function ($) {
  Drupal.behaviors.uwpeople = function (context) {
    var _url = Drupal.settings.uwpeople.destination_path;
    
    $("input#edit-grouping-reload").click(function (event) {
      event.preventDefault();
      window.location = _url;
    });
  };
})(jQuery);
