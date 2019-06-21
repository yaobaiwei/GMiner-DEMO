// GLOBAL VARIABLE //
var graphEnv = {force: undefined, svg: undefined};
var ENV = { 
  stdpt: 0 , key: undefined, timeid: undefined, apps: undefined, // reset in changeComponents
  chart: undefined, 
  removed_edges: undefined, removed_nodes: undefined, seed_id: undefined // reset in renderGraphVisualize
};
// Graph //
var node_menu = function(){
  if(graphEnv.force.nodes().length > 1){
    return [{
      title: 'Remove Node',
      action: function(elem, d, i) {
        /*
         * elem: the element occur the menu
         * d: data bound to the elem
         * i: index
         */
        if (d.id == ENV.seed_id) {
          console.log("d.id == ENV.seed_id");
          var x = document.getElementById("toast")
          x.className = "show";
          setTimeout(function(){ x.className = x.className.replace("show", ""); }, 2000);
          return;
        }
        console.log('delete node:',d);
        var svg = graphEnv.svg,
            force = graphEnv.force,
            nodes = force.nodes(),
            edges = force.force('link').links();
        force.stop();

        edges = edges.filter(function(e) {
          return e.source != d && e.target != d;
        });
        nodes.splice(i, 1);

        force.force('link').links(edges);
        bindAndAlign(svg.selectAll('circle'), nodes, svg.selectAll('line'), edges);
        if(ENV.apps === "mc"){
          stylizeMcGraph();
        }
        else if(ENV.apps === "fco"){
          stylizeFcoGraph();
        }
        else {
          stylizeNormalGraph();
        }
        force.restart();

        if(typeof ENV.removed_nodes === "undefined")
          ENV.removed_nodes = [d.id];
        else
          ENV.removed_nodes.push(d.id);

        d3.event.stopPropagation();
      }
    }];
  }
  else{
    return [{
      title: 'Cannot remove the last node of the task',
      action: (a,b,c)=>{},
      disabled: true
    }];
  }
};
var edge_menu = [{
  title: 'Remove Edge',
  action: function(elem, d, i){
    console.log('delete edge: ', d);
    var svg = graphEnv.svg,
        force = graphEnv.force,
        nodes = force.nodes(),
        edges = force.force('link').links();
    force.stop();
    edges.splice(i, 1);
    bindAndAlign(svg.selectAll('circle'), nodes, svg.selectAll('line'), edges);
    if(ENV.apps === "mc"){
      stylizeMcGraph();
    }
    else if(ENV.apps === "fco"){
      stylizeFcoGraph();
    }
    else {
      stylizeNormalGraph();
    }
    force.restart();
    if(typeof ENV.removed_edges === "undefined")
      ENV.removed_edges = [[d.source.id, d.target.id]];
    else
      ENV.removed_edges.push([d.source.id, d.target.id]);
    d3.event.stopPropagation();
  }
}];
// FORM FIELDS //
var cache_field = {
  identifier: 'cache-size',
  rules: [{ type: 'integer[10000..2147483647]'}]
};
var comp_th_field = {
  identifier: 'num-comp-thread',
  rules: [{type: 'integer[1..22]'}]
};
var pipe_pop_field = {
  identifier: 'pipe-pop-num',
  rules: [{type: 'integer[100..5000]'}]
};
var pop_field = {
  identifier: 'pop-num',
  rules: [{type: 'integer[50..5000]'}]
}
var subg_field = {
  identifier: 'subg-size-t',
  rules: [{type: 'integer[10..100]'}]
};
var gc_mweight_field = {
  identifier: 'min-weight',
  optional: true,
  rules: [{type: 'regExp', value: /^(0+\.?|0*\.\d+|0*1(\.0*)?)$/}]
};
var gc_diff_ratio_field = {
  identifier: 'diff-ratio',
  optional: true,
  rules: [{type: 'regExp', value: /^(0+\.?|0*\.\d+|0*1(\.0*)?)$/}]
};
var gc_mcore_field = {
  identifier: 'min-core-size',
  rules: [{type: 'integer[1..100]'}]
};
var gc_mres_field = {
  identifier: 'min-result-size',
  rules: [{type: 'integer[1..1000]'}]
};
var gc_iter_field = {
  identifier: 'iter-round-max',
  rules: [{type: 'integer[1..3000]'}]
};
var gc_cand_field = {
  identifier: 'cand-max-time',
  rules: [{type: 'integer[1..3000]'}]
};
var tc_sampling_min = {
  identifier: 'tc-sampling-min',
  rules: [{type: 'integer[4..200]'}]
};
var tc_sampling_max = {
  identifier: 'tc-sampling-max',
  rules: [{type: 'integer[4..200]'}]
};
var gm_sampling_min = {
  identifier: 'gm-sampling-min',
  rules: [{type: 'integer[4..200]'}]
};
var gm_sampling_max = {
  identifier: 'gm-sampling-max',
  rules: [{type: 'integer[4..200]'}]
};
var cd_sampling_min = {
  identifier: 'cd-sampling-min',
  rules: [{type: 'integer[4..100]'}]
};
var cd_sampling_max = {
  identifier: 'cd-sampling-max',
  rules: [{type: 'integer[4..100]'}]
};
var gc_sampling_min = {
  identifier: 'gc-sampling-min',
  rules: [{type: 'integer[4..100]'}]
};
var gc_sampling_max = {
  identifier: 'gc-sampling-max',
  rules: [{type: 'integer[4..100]'}]
};
var cd_thre_field = {
  identifier: 'k-threshold',
  rules: [{type: 'integer[2..10000]'}]
};
var has_default_fields = {
  cache: cache_field,
  thread: comp_th_field,
  pipe: pipe_pop_field,
  pop: pop_field,
  subg: subg_field,
  cd:cd_thre_field,
  min_weight: gc_mweight_field,
  diffr: gc_diff_ratio_field,
  mcore: gc_mcore_field,
  mres: gc_mres_field,
  iter: gc_iter_field,
  cand: gc_cand_field,
  tc_min: tc_sampling_min,
  tc_max: tc_sampling_max,
  gm_min: gm_sampling_min,
  gm_max: gm_sampling_max,
  cd_min: cd_sampling_min,
  cd_max: cd_sampling_max,
  gc_min: gc_sampling_min,
  gc_max: gc_sampling_max
}

