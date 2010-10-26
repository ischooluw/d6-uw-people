<?php

/***************************************************************
 * UW People Module                                            *
 *                                                             *
 * Created by the University of Washington Information School. *
 * Be sure to read README and LICENSE.                         *
 ***************************************************************/

/**
 * @file
 * Custom CRON script for the UW People module.  This is defined as a
 * separate script as pulling this data can take some time.
 */

include_once '../../../../includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

drupal_load('module', 'uwpeople');
_uwpeople_reload_all();
