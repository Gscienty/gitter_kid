import React from 'react';
import ProjectTemplate from './_project';
import { connect } from 'react-redux';

class Page extends ProjectTemplate {
    componentDidMount() {
        let path = '';
        let prePaths = this.props.match.url.split('/tree');
        if (prePaths.length === 2) {
            path += prePaths[1];
        }
        else {
            path = '/'
        }
        this.props.dispatch({
            type: 'tree/getTree',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName: this.props.match.params.branchName,
                path
            }
        });
    }

    unit() {

        return <div>HERE</div>;
    }
}

export default connect(
    ({ tree }) => ({
        entryUnit: 'repositories', projectEntryUnit: 'code',
        tree
    })
)(Page);