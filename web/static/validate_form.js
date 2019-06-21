function stopAll() {
  $('#runButton').removeClass('disabled');
  $('#stopButton,#pauseButton').addClass('disabled');
  $('#pauseButton').show();
  $('#resumeButton').hide();
  $('#graphPanel .dimmer').removeClass('active');

  clearQueue();
  $('#queues .progress').addClass('disabled');
  $('.arrows i').removeClass('move');

  if(typeof(ENV.timeid) != "undefined"){
    clearTimeout(ENV.timeid);
    ENV.timeid = undefined;
  }

  ENV.current_status = undefined;

  console.log("function stopAll()");
}

function clearQueue(){
  $('#pq').progress({
    text: {percent: String(0) },
    percent: Number(0) * 100
  });
  $('#pq .arrow-label').text(String(0));
  $('#cmq').progress({
    text: {percent: String(0)},
    percent: Number(0) * 100
  });
  $('#cpq').progress({
    text: {percent: String(0)},
    percent: Number(0) * 100
  });
  $('#qlabel1').text(String(0));
  $('#qlabel2').text(String(0));
  $('#qlabel3').text(String(0));
  $('#qlabel4').text(String(0));
}

function renderComponents(data){
  // 1. flush console
  let stdout = data['text'];
  let text = $('#stdConsole>p').html();
  $('#stdConsole>p').html(text + stdout);
  if($('#contentMenu>.item.active').attr('group') === 'std'){
    $('#stdConsole').scrollTop($('#stdConsole')[0].scrollHeight);
  }
  data['text'] = '';
  // console.log('[renderComponents] ', data);
  ENV.stdpt = data.stdpt;

  arrow_label_suffix = ' Tasks/sec';
  // 2. flush queues
  $('#pq').progress({
    text: {percent: String(data['task_num_in_disk']) },
    percent: Number(data['task_num_in_disk_float']) * 100
  });
  $('#pq .arrow-label').text(String(data['task_transfer_1']));
  $('#cmq').progress({
    text: {percent: String(data['cmq_size'])},
    percent: Number(data['cmq_size_float']) * 100
  });
  $('#cpq').progress({
    text: {percent: String(data['cpq_size'])},
    percent: Number(data['cpq_size_float']) * 100
  });
  $('#qlabel1').text(String(data['task_transfer_1']));
  $('#qlabel2').text(String(data['task_transfer_2']));
  $('#qlabel3').text(String(data['task_transfer_3']));
  $('#qlabel4').text(String(data['task_transfer_4']));

  if (data["hdfsLoaded"]) {
    $('#graphPanel .text.loader').text('No sampled result...');
  }

  if (ENV.paused != "undefined")
    renderGraphVisualize(data['taskRes']);
  return data['end'];  
}

function confirmContinue(is_end){
  if(is_end == '1'){
    $('#statusTag').text('Job Finished .').show();
    stopAll();
    return;
  }
  ENV.timeid = setTimeout(manageInteraction, 500);
}

function manageInteraction(){
  // console.log('[manageInteraction]');
  let request = {"key": ENV.key, "stdpt": ENV.stdpt};
  let url = '/interaction';
  fetch(url, {
    method: 'POST',
    body: JSON.stringify(request),
    headers: new Headers({'Content-Type': 'application/json'})
  })
  .then(resp => resp.json())
  .then(renderComponents)
  .then(confirmContinue)
  .catch(err => console.error('Interaction error', err));
}

function changeComponents(data){
  if(data.status === "ok"){
    $('#stopButton').removeClass('disabled');
    // $('#pauseButton').removeClass('disabled');
    $('#queues .progress').removeClass('disabled');
    $('#graphPanel .text.loader').text('Loading graph from HDFS...');
    $('.arrows i').addClass('move');
    d3.select('#maingraph').selectAll('*').remove();
    if(data["apps"] === "gm"){
      $('#gmgt').show();      
    }
    else {
      $('#gmgt').hide();      
    }
    ENV.key = data.key;
    ENV.timeid = setTimeout(manageInteraction, 500);
    ENV.apps = data.apps;
    ENV.stdpt = 0;
    return;
  }
  alert('[changeComponents] Run command fail, please reset parameters and try again!');
  throw "changeComponents error";
}

function resumeInteraction(data) {
  console.log('[resumeInteraction] ',data);
  if(data.status === "ok"){
    $('#queues .progress').removeClass('disabled');
    $('.arrows i').addClass('move');
    ENV.timeid = setTimeout(manageInteraction, 500);
    return;
  }
  else if(data.status === "finished"){
    $('#resumeModal').modal('show');    
  }
  alert('[resumeInteraction] Resume command fail, please reset parameters and try again!');
  throw "changeComponents error";
}
// life cycle start
function submitRunForm(fields){
  console.log('[submitRunForm] start submit');

  $('#stdConsole>p').text('');
  $('#graphnote').hide();
  $('#graphnote *').remove();
  $('#statusTag').hide();
  
  $('#runButton').addClass('disabled');
  $('#graphPanel .dimmer').addClass('active');
  
  var url = '/runrequest';
  var data = JSON.stringify(fields);
  console.log('[submitRunForm]',data);
  fetch(url, {
    method: 'POST',
    body: data,
    headers: new Headers({'Content-Type': 'application/json'})
  })
  .then(resp => resp.json())
  .then(changeComponents)
  .then(ENV.current_status = 2)
  .catch(function(e) {
    submitStopRequest();
    stopAll();
    $('#graphPanel .dimmer').removeClass('active');
  });
}

function submitResumeRequest() {
  ENV.paused = undefined;
  let url = '/resumerequest';
  let resume_req = {'key': ENV.key };
  if(typeof ENV.removed_nodes == "undefined"
    && typeof ENV.removed_edges == "undefined"){
    // 1. run as normal
    $('#pauseButton').show();
    resume_req.seed_id = -1;
  }
  else{
    $('#pauseButton').addClass('disabled').show();
    resume_req.removed_nodes = ENV.removed_nodes;
    resume_req.removed_edges = ENV.removed_edges;
    resume_req.seed_id = ENV.seed_id;
  }
  console.log('[submitResumeRequest] ', resume_req)
  fetch(url, {
    method: "POST",
    body: JSON.stringify(resume_req),
    headers: new Headers({'Content-Type': 'application/json'})
  })
  .then(resp => resp.json())
  .then(resumeInteraction)
  .catch(e => {submitStopRequest(); stopAll();});
}

function submitStopRequest() {
  if(typeof ENV.key != "undefined"){
    let stop_req = { "key": ENV.key };
    let url = '/stoprequest';
    // console.log(ENV.key, " request stop");
    $(this).addClass('disabled');
    fetch(url, {
      method: 'POST',
      body: JSON.stringify(stop_req),
      headers: new Headers({'Content-Type': 'application/json'})
    })
    .catch(error => {console.log('stop failed'); throw error;});
  }
}

function validateFormWithDefault() {
  let now_field_values = $('#config .ui.form').form('get values');
  // console.log(now_field_values);
  for(let v in has_default_fields){
    let field = has_default_fields[v];
    let now_val = now_field_values[field.identifier];
    if(now_val === ""){
      let selector = '#' + field.identifier;
      let val = $(selector).attr('placeholder');
      $(selector).attr('value', val);
    }
  }
  $('.ui.form').form('validate form');
  return $('.ui.form').form('is valid');
}
function updateTableInfo() {
  let now_field_values = $('#config .ui.form').form('get values');
  for(let v in now_field_values){
    let selector = '#table' + v;
    let table_item = $(selector);
    let input_selector = '#' + v;
    
    if(v === "ib"){
      let val;
      if(now_field_values[v]) val="Infiniband";
      else val = "Ethernet";
      table_item.text(val);
    }
    else if(now_field_values[v] === ""){
      let val = $(input_selector).attr('placeholder');
      if(typeof(val)=="undefined"){
        val = "unset";
      }
      else{
        val = val + ' (default)';
      }
      table_item.text(val);
    }
    else if(v === "apps"){
      let opt = $('#apps option:selected');
      let val = opt.text();
      table_item.text(val);
    }
    else{
      table_item.text(now_field_values[v]);
    }
  }
}
$(document).ready(function(){
  /* initialize */
  $('.ui.form').form({
    fields: {
      apps: 'empty',
      dataset: 'empty'
    },
    duration: 800
  });
  $('.ui.form').form('add fields', has_default_fields);
  /* actions */
  $('#resumeModal').modal({inverted: true, blurring: true});
  $('#configModal').modal({
    onApprove: validateFormWithDefault,
    onHidden: updateTableInfo
  });

  $('#runButton').click(function(){
    if($(this).hasClass('.disabled')){
      return;
    }
    let res = validateFormWithDefault();
    if(res === true) {
      submitRunForm($('#config .ui.form').form('get values'));
    }
    else{
      $('#configModal').modal('show');
    }
  });

  // bind stop
  $('#stopButton').on('click', function(){
    if(!$(this).hasClass('disabled')){
      submitStopRequest();
      stopAll();
    }
  });

  $('#pauseButton').on('click', function(){
    if(!$(this).hasClass('disabled')){
      $(this).hide();
      $('#resumeButton').show();
      $('#runButton').addClass('disabled');
      
      // $('#queues .progress').addClass('disabled');
      $('.arrows i').removeClass('move');

      let pause_req = {"key": ENV.key };
      let url = '/pauserequest';
      fetch(url, {
        method: "POST",
        body: JSON.stringify(pause_req),
        headers: new Headers({'Content-Type': 'application/json'})
      })
      .catch(error => {console.log('pause failed'); throw error;});

      ENV.paused = 1;
      // if(typeof(ENV.timeid) != "undefined"){
      //   clearTimeout(ENV.timeid);
      //   ENV.timeid = undefined;
      // }
    }
  });

  $('#resumeButton').on('click', function(){
    $(this).hide();
    submitResumeRequest();
  });
});
